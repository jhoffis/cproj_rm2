const std = @import("std");
const print = std.debug.print;
const testing = std.testing;
const rep = @import("rep.zig");
const Rep = rep.Rep;
const Aspects = rep.Aspects;

pub const Stats = struct {
    throttle: bool, 
    clutch: bool, 
    NOSON: bool, 
    tireboostON: bool, 
    NOSDownPressed: bool,
    sequentialShift: bool, 
    soundBarrierBroken: bool, 
    hasDoneStartBoost: bool, 
    turboBlowON: bool, 
    redlinedThisGear: bool, 
    grinding: bool, 

    nosBottleAmountLeft: u32,
    gear: u32,
    finishSpeed: u32,

    tireboostTimeLeft: u64,
    lastTimeReleaseThrottle: u64,
    grindingTime: u64,

    speed: f64, 
    distance: f64, 
    spool: f64, 
    spdinc: f64, 
    brake: f64, 
    throttlePercent: f64,
    clutchPercent: f64, // If < 1 then clutch engaged
    rpm: f64, 
    rpmGoal: f64, 
    grindingCurrent: f64,
    percentTurboSupercharger: f64,

    nosTimesLeft: [24]u64,
    nosTimesFrom: [24]u64, // FIXME ikke uendelig mengde nos
    
    stats: [@typeInfo(Aspects).Enum.fields.len]f64 = undefined,
};

pub var tempstats: Stats = undefined;
pub const lenNos = 24;

pub fn reset(stats: *Stats, r: *Rep, numSuperchargers: f64, numTurbos: f64) void {
    for (r.stats, 0..) |value, i| {
        stats.stats[i] = value;
    }

    if (numSuperchargers == 0) {
        stats.percentTurboSupercharger = 1;
    } else if (numSuperchargers > numTurbos) {
        stats.percentTurboSupercharger = numTurbos / numSuperchargers;
    } else {
        stats.percentTurboSupercharger = numSuperchargers / numTurbos;
    }

    stats.turboBlowON = false;
    stats.NOSDownPressed = false;
    //Med space
    //		clutch = false;
    //		resistance = 0f;
    // Uten space
    stats.grindingTime = 0;
    stats.grindingCurrent = 1;
    stats.throttlePercent = 0;
    stats.clutch = true;
    stats.clutchPercent = 1;
    stats.lastTimeReleaseThrottle = 0;
    stats.redlinedThisGear = false;
    stats.throttle = false;
    stats.NOSON = false;
    stats.soundBarrierBroken = false;
    stats.hasDoneStartBoost = false;
    stats.nosBottleAmountLeft = @intFromFloat(stats.stats[@intFromEnum(Aspects.nosBottles)]);
    var i: usize = 0;
    while (i < lenNos) : (i += 1) { // FIXME hardkodet verdi på max mengde nos
        stats.nosTimesLeft[i] = 0;
        stats.nosTimesFrom[i] = 0;
    }
    stats.gear = 1;
    stats.speed = 0;
    stats.distance = 0;
    stats.spool = stats.stats[@intFromEnum(Aspects.spoolStart)];
    stats.spdinc = 0;
    stats.rpm = stats.stats[@intFromEnum(Aspects.rpmIdle)];
    stats.rpmGoal = stats.rpm;
    stats.tireboostTimeLeft = 0;
    stats.sequentialShift = rep.is(r, Aspects.sequential);

    stats.brake = 0;
}

pub fn stat(stats: *Stats, aspect: Aspects) f64 {
    return stats.stats[@intFromEnum(aspect)];
}

pub fn statu64(stats: *Stats, aspect: Aspects) u64 {
    return @intFromFloat(stats.stats[@intFromEnum(aspect)]);
}

pub fn setStat(stats: *Stats, aspect: Aspects, val: f64) void {
    stats.stats[@intFromEnum(aspect)] = val;
}

pub fn hasTurbo(stats: *Stats) bool {
    return stat(stats, Aspects.bar) > 0;
}

pub fn hasTwoStep(stats: *Stats) bool {
    return stat(stats, Aspects.twoStep) != 0;
}

pub fn nosPercentageLeft(s: *Stats, i: u32, now: u64) f32 {
    if (i >= lenNos or s.nosTimesLeft[i] == 0)
        return 1.0;
    if (s.nosTimesLeft[i] > now) {
        const timespan: u64 = s.nosTimesLeft[i] - now;
        return @as(f32, @floatFromInt(timespan)) / 
                        @as(f32, @floatFromInt(s.nosTimesLeft[i] - s.nosTimesFrom[i]));
    }
    return 0;
}

export fn getNosPercentageLeft(i: u32, now: u64) f32 {
    return nosPercentageLeft(&tempstats, i, now);
}
	
pub fn tbPercentageLeft(s: *Stats, now: u64) f64 {
    const tbMaxTime: f64 = stat(s, Aspects.tbMs);
    if (s.tireboostTimeLeft <= now) {
        return 0;
    }
    const tbTime: f64 = @floatFromInt(s.tireboostTimeLeft - now);
    const percent = tbTime / tbMaxTime;
    // std.debug.print("maxtime: {}, timeleft: {}, tbTime: {}, now: {}, percent: {}\n", .{tbMaxTime, s.tireboostTimeLeft, tbTime, now, percent});
    return @max(percent, 0);
}

export fn getTBPercentageLeft(now: u64) f64 {
    return tbPercentageLeft(&tempstats, now);
}

fn popNosBottle(fromTime: u64, tillTime: u64) void {
    const s = &tempstats;

    var i: u8 = 0;
    while (i < lenNos) : (i += 1) {
        if (s.nosTimesFrom[i] != 0) continue;

        s.nosTimesFrom[i] = fromTime;
        s.nosTimesLeft[i] = tillTime;
        s.nosBottleAmountLeft -= 1;
        return;
    }
}
	
export fn nos(fromTime: u64) bool {
    const s = &tempstats;
    if (s.nosBottleAmountLeft > 0) {
        popNosBottle(fromTime, fromTime + statu64(s, Aspects.nosMs));
        return true;
    }
    return false;
}

pub fn getNosTimeLeft(s: *Stats, i: usize) u64 {
    if (i < lenNos) {
        return s.nosTimesLeft[i];
    }
    return 0;
}

export fn changeLastNosTimeLeft(val: u64) void {
    // OLD SOLUTION: if (nosTimesLeft.size() > 0)
    //     nosTimesLeft.set(nosTimesLeft.size() - 1, nosTimesLeft.get(nosTimesLeft.size() - 1) + val);
    const s = &tempstats;
    if (s.nosTimesFrom[0] == 0) return; 

    var i: u8 = 1;
    while (i < lenNos) : (i += 1) {
        if (s.nosTimesFrom[i] != 0) continue;
        s.nosTimesLeft[i - 1] += val;
    }
}

//////////////

export fn getStat(aspect: u32) f64 {
    return tempstats.stats[aspect];
}

export fn setStatEx(aspect: u32, val: f64) void {
    tempstats.stats[aspect] = val;
}

export fn isThrottle() bool {
    return tempstats.throttle;
}

export fn setThrottle(val: bool) void {
    tempstats.throttle = val;
}

export fn isClutch() bool {
    return tempstats.clutch;
}

export fn setClutch(val: bool) void {
    tempstats.clutch = val;
}

export fn isNosON() bool {
    return tempstats.NOSON;
}

export fn setNosON(val: bool) void {
    tempstats.NOSON = val;
}

export fn isTbON() bool {
    return tempstats.tireboostON;
}

export fn isTbRight() bool {
    return tempstats.tireboostON and tempstats.throttle;
}

export fn setTbON(val: bool) void {
    tempstats.tireboostON = val;
}

export fn isTurboblowON() bool {
    return tempstats.turboBlowON;
}

export fn setTurboblowON(val: bool) void {
    tempstats.turboBlowON = val;
}

export fn isNOSDownPressed() bool {
    return tempstats.NOSDownPressed;
}

export fn setNOSDownPressed(val: bool) void {
    tempstats.NOSDownPressed = val;
}

export fn isSequentialShift() bool {
    return tempstats.sequentialShift;
}

export fn setSequentialShift(val: bool) void {
    tempstats.sequentialShift = val;
}

export fn isSoundBarrierBroken() bool {
    return tempstats.soundBarrierBroken;
}

export fn setSoundBarrierBroken(val: bool) void {
    tempstats.soundBarrierBroken = val;
}

export fn isHasDoneStartBoost() bool {
    return tempstats.hasDoneStartBoost;
}

export fn setHasDoneStartBoost(val: bool) void {
    tempstats.hasDoneStartBoost = val;
}

export fn isRedlinedThisGear() bool {
    return tempstats.redlinedThisGear;
}

export fn setRedlinedThisGear(val: bool) void {
    tempstats.redlinedThisGear = val;
}

export fn isGrinding() bool {
    return tempstats.grinding;
}

export fn setGrinding(val: bool) void {
    tempstats.grinding = val;
}

export fn getNosBottleAmountLeft() u32 {
    return tempstats.nosBottleAmountLeft;
}

export fn setNosBottleAmountLeft(val: u32) void {
    tempstats.nosBottleAmountLeft = val;
}

export fn getGear() u32 {
    return tempstats.gear;
}

export fn setGear(val: u32) void {
    tempstats.gear = val;
}

export fn getFinishSpeed() u32 {
    return tempstats.finishSpeed;
}

export fn setFinishSpeed(val: u32) void {
    tempstats.finishSpeed = val;
}

export fn getTireboostTimeLeft() u64 {
    return tempstats.tireboostTimeLeft;
}

export fn setTireboostTimeLeft(val: u64) void {
    tempstats.tireboostTimeLeft = val;
}

export fn getLastTimeReleaseThrottlet() u64 {
    return tempstats.lastTimeReleaseThrottle;
}

export fn setLastTimeReleaseThrottle(val: u64) void {
    tempstats.lastTimeReleaseThrottle = val;
}

export fn getGrindingTime() u64 {
    return tempstats.grindingTime;
}

export fn setGrindingTime(val: u64) void {
    tempstats.grindingTime = val;
}

export fn setSpeed(val: f64) void {
    tempstats.speed = val;
}

export fn getSpeed() f64 {
    return tempstats.speed;
}

export fn setDistance(val: f64) void {
    tempstats.distance = val;
}

export fn getDistance() f64 {
    return tempstats.distance;
}

export fn setSpool(val: f64) void {
    tempstats.spool = val;
}

export fn getSpool() f64 {
    return tempstats.spool;
}

export fn setSpdInc(val: f64) void {
    tempstats.spdinc = val;
}

export fn getSpdInc() f64 {
    return tempstats.spdinc;
}

export fn setBrake(val: f64) void {
    tempstats.brake = val;
}

export fn getBrake() f64 {
    return tempstats.brake;
}

export fn setThrottlePercent(val: f64) void {
    tempstats.throttlePercent = val;
}

export fn getThrottlePercent() f64 {
    return tempstats.throttlePercent;
}

export fn setClutchPercent(val: f64) void {
    tempstats.clutchPercent = val;
}

export fn getClutchPercent() f64 {
    return tempstats.clutchPercent;
}

export fn setRpm(val: f64) void {
    tempstats.rpm = val;
}

export fn getRpm() f64 {
    return tempstats.rpm;
}

export fn setRpmGoal(val: f64) void {
    tempstats.rpmGoal = val;
}

export fn getRpmGoal() f64 {
    return tempstats.rpmGoal;
}

export fn setGrindingCurrent(val: f64) void {
    tempstats.grindingCurrent = val;
}

export fn getGrindingCurrent() f64 {
    return tempstats.grindingCurrent;
}

export fn setPercentTurboSupercharger(val: f64) void {
    tempstats.percentTurboSupercharger = val;
}

export fn getPercentTurboSupercharger() f64 {
    return tempstats.percentTurboSupercharger;
}







test "create a stats" {
    const id = rep.createTemp();
    const r = &rep.reps[id];
    var stats: Stats = undefined;

    reset(&stats, r, 0, 0);

    try testing.expectEqual(stats.gear, 1);

    stats.nosTimesLeft[0] = 123;

    try testing.expectEqual(stats.nosTimesLeft[0], 123);


    const nosPerc = nosPercentageLeft(&stats, 0, 1722201626094);	
    const nosPerc2 = nosPercentageLeft(&stats, 1, 1722201626094);	
    const ranNos = nos(1722201626094);
    changeLastNosTimeLeft(1000);
    
    try testing.expectEqual(0, nosPerc);
    try testing.expectEqual(1, nosPerc2);
    try testing.expect(!ranNos);
}
    
test "nos percentage" {
    var stats: Stats = undefined;
    stats.nosTimesLeft[0] = 1100;
    stats.nosTimesFrom[0] = 0;
    const nosPerc2 = nosPercentageLeft(&stats, 0, 1000);
    try testing.expectEqual(100.0/1100.0, nosPerc2);
    try testing.expect(nosPerc2 != 1 and nosPerc2 != 0);
}

test "tb percentage" {
    var stats: Stats = undefined;
    setStat(&stats, Aspects.tbMs, 500);
    stats.tireboostTimeLeft = 1100;
    var tbPerc = tbPercentageLeft(&stats, 1000);
    try testing.expectEqual(100.0/500.0, tbPerc);

    tbPerc = tbPercentageLeft(&stats, 2000);
    try testing.expectEqual(0, tbPerc);
}











