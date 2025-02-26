const std = @import("std");
const print = std.debug.print;
const testing = std.testing;
const strb = @import("strbuilder.zig");

pub const Aspects = enum {
    hp, 
    kg,
    bar,
    spdTop,
    rpmTop,
    rpmIdle,
    turboblow,
    nos,
    nosMs,
    tb,
    tbMs,
    // tbLongifySpell, // TODO make up some stuff
    // tbJumpSpell,
    spool,
    spoolStart,
    aero,
    turboblowRegen,
    turboblowStrength,
    moneyPerTurn,
    interest,
    // cannot be multiplied:
    nosBottles,
    spdTopBase, 
    rpmBaseIdle,
    rpmBaseTop,
    snos,
    life,
    gearTop,
    nosAuto,
    nosPush,
    tbArea ,
    stickyclutch,
    sequential,
    nosSoundbarrier,
    manualClutch,
    twoStep, // FIXME Why is this here? It is a bool value that should not be added through upgrades... What is this struct really??? Isn'ẗ it the representation of your car? And
             // so on....
    throttleShift,
    highestSpdAchived, // FIXME Why is this here? HighestSpeed cannot be upgraded?!
};

pub const Rep = struct {
    nameId: i8,
    random: bool,
    stats: [@typeInfo(Aspects).Enum.fields.len]f64 = undefined,
};

pub var reps: [24]Rep = undefined; // TODO have a self-expanding array
pub var repsUsed: [24]bool = undefined;

pub fn createTemp() u8 {
    var repId: u8 = 0;
    var found: bool = false;
    for (reps, 0..) |_, i| {
        if (repsUsed[i]) continue;
        repId = @intCast(i); 
        repsUsed[i] = true;
        found = true;
        break;
    }
    print("creating repsused: {any}\n", .{repsUsed});
    if (!found) {
        print("DIDN'T FIND AVAILABLE REP!", .{});
    }
    return repId;
}

export fn createTempRep() u8 {
    return createTemp();
}

export fn createRep(nameId: i8, random: bool) u8 {
    const repId: u8 = createTemp();
    reps[repId] = Rep {
        .nameId = nameId,
        .random = random,
    };

    print("hello\n", .{});

    set(repId, Aspects.spool, 1);
    set(repId, Aspects.turboblow, 100);
    set(repId, Aspects.turboblowStrength, 1);
    set(repId, Aspects.turboblowRegen, 25);
    set(repId, Aspects.aero, 1);
    set(repId, Aspects.interest, 0.2);

    switch (nameId) {
        0 => {
            set(repId, Aspects.hp, 1330);
            set(repId, Aspects.kg, 1650);
            set(repId, Aspects.bar, 1.2);
            set(repId, Aspects.turboblowStrength, 3);
            set(repId, Aspects.rpmTop, 5500);
            set(repId, Aspects.rpmIdle, 5500.0 / 4.5);
            set(repId, Aspects.gearTop, 4);
            set(repId, Aspects.spdTop, 155);
            set(repId, Aspects.tbMs, 900);
            set(repId, Aspects.nosMs, 500);
        },
        1 => {
            set(repId, Aspects.hp, 2840);
            set(repId, Aspects.kg, 3050);
            set(repId, Aspects.rpmTop, 6500);
            set(repId, Aspects.rpmIdle, 6500.0 / 8.0);
            set(repId, Aspects.gearTop, 3);
            set(repId, Aspects.spdTop, 150);
            set(repId, Aspects.tbArea, 1);
            set(repId, Aspects.tb, 1);
            set(repId, Aspects.tbMs, 1350);
            set(repId, Aspects.nosMs, 500);
        },
        2 => {
            set(repId, Aspects.hp, 900);
            set(repId, Aspects.kg, 1215);
            set(repId, Aspects.rpmTop, 10000);
            set(repId, Aspects.rpmIdle, 10000.0 / 8.0);
            set(repId, Aspects.gearTop, 5);
            set(repId, Aspects.spdTop, 150);
            set(repId, Aspects.tbMs, 900);
            set(repId, Aspects.nosMs, 600);
            set(repId, Aspects.nosBottles, 1);
            set(repId, Aspects.nos, 1.4);
        },
        3 => {
            set(repId, Aspects.hp, 1120);
            set(repId, Aspects.kg, 1450);
            set(repId, Aspects.rpmTop, 5000);
            set(repId, Aspects.rpmIdle, 5000.0 / 4.0);
            set(repId, Aspects.gearTop, 6);
            set(repId, Aspects.spdTop, 150);
            set(repId, Aspects.tbMs, 700);
            set(repId, Aspects.nosMs, 400);
        },
        else => {
            // TODO throw error or something
        },
    }
    set(repId, Aspects.rpmBaseTop, get(repId, Aspects.rpmTop));
    set(repId, Aspects.spdTopBase, get(repId, Aspects.spdTop));
    return repId;
}


export fn destroyRep(repId: u8) void {
    repsUsed[repId] = false;
    print("destroying repsused: {any}\n", .{repsUsed});
}

export fn destroyAllReps() void {
    for (repsUsed, 0..) |_, i| {
        repsUsed[i] = false;
    }
    print("destroying all repsused: {any}\n", .{repsUsed});
}

pub fn reset(rep: *Rep) void {
    if (getStat(rep, Aspects.kg) < 50) {
        setStat(rep, Aspects.kg, 50); // sjåføren
    }

    if (getStat(rep, Aspects.aero) < 0) {
        setStat(rep, Aspects.aero, 0);
    }

    // if (!rep.is(Rep.sequential) && rep.get(Rep.gearTop) > 10) {
    //     rep.set(Rep.gearTop, 10);
    // }

    // if (Double.isNaN(rep.get(Rep.kW))) {
    //     rep.set(Rep.kW, Double.MAX_VALUE);
    // }

    if (getStat(rep, Aspects.rpmIdle) > getStat(rep, Aspects.rpmTop) - 100) {
        setStat(rep, Aspects.rpmIdle, getStat(rep, Aspects.rpmTop) - 100);
    }

    // for (int i = 0; i < Rep.tbArea; i++) {
    //     if (rep.get(i) < 0d)
    //         rep.set(i, 0d);
    // }

    if (getStat(rep, Aspects.spdTop) >= std.math.pow(f64, 10.07925285, 9)) {
        setStat(rep, Aspects.spdTop, std.math.pow(f64, 10.07925285, 9));
    }
}

export fn setRepStats(repId: u8, aspect: u32, val: f64) void {
    reps[repId].stats[aspect] = val;
}

export fn isRandom(repId: u8) bool {
    return reps[repId].random;
}

export fn setRandom(repId: u8, val: bool) void {
    reps[repId].random = val;
}

export fn getNameId(repId: u8) i8 {
    return reps[repId].nameId;
}

export fn setNameId(repId: u8, nameId: i8) void {
    reps[repId].nameId = nameId;
}

pub fn turboHP(r: *Rep) f64 {
    return getStat(r, Aspects.hp) / 4.0 * getStat(r, Aspects.bar);
}

export fn getTurboHP(repId: u8) f64 {
    return turboHP(&reps[repId]);
}

pub fn totalHP(r: *Rep) f64 {
    const hp = getStat(r, Aspects.hp);
    const thp = turboHP(r);
    const rpmT = getStat(r, Aspects.rpmTop);
    const rpmBT = getStat(r, Aspects.rpmBaseTop);

    var a = (hp + thp) * (rpmT / rpmBT);

    if (std.math.isInf(a)) {
        a = std.math.sign(a) * std.math.floatMax(f64);
    }

    return a;
}

export fn getTotalHP(repId: u8) f64 {
    const hp = get(repId, Aspects.hp);
    const thp = getTurboHP(repId);
    const rpmT = get(repId, Aspects.rpmTop);
    const rpmBT = get(repId, Aspects.rpmBaseTop);

    var a = (hp + thp) * (rpmT / rpmBT);

    if (std.math.isInf(a)) {
        a = std.math.sign(a) * std.math.floatMax(f64);
    }

    return a;
}

pub fn updateHighestSpeed(r: *Rep, currentSpeed: f64) void {
    if (currentSpeed > getStat(r, Aspects.highestSpdAchived))
        r.stats[@intFromEnum(Aspects.highestSpdAchived)] = currentSpeed;
}

pub fn set(repId: u8, aspect: Aspects, val: f64) void {
    reps[repId].stats[@intFromEnum(aspect)] = val;
}

pub fn setStat(r: *Rep, aspect: Aspects, val: f64) void {
    r.stats[@intFromEnum(aspect)] = val;
}

pub fn get(repId: u8, aspect: Aspects) f64 {
    return reps[repId].stats[@intFromEnum(aspect)];
}

pub fn getStat(r: *Rep, aspect: Aspects) f64 {
    return r.stats[@intFromEnum(aspect)];
}

pub fn createRepCloneString(repId: u8, input: []u8) ![]const u8 {
    const rep = reps[repId];

    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    var clstr: []const u8 = strb.splitter;
    if (rep.random) {
        clstr = try strb.appendNum(arena.allocator(), clstr, -rep.nameId - 1);
    } else {
        clstr = try strb.appendNum(arena.allocator(), clstr, rep.nameId);
    }
    for (clstr, 0..) |v, i| {
       input[i] = v; 
    }

    return input;

    // for (int i = 0; i < stats.length; i++) {
    //
    //     if (Double.isInfinite(stats[i])) {
    //         stats[i] = Math.signum(stats[i]) * Double.MAX_VALUE;
    //     }
    //
    //     outString
    //         .append(splitter)
    //         .append(stats[i]);
    // }
    // outString.append(splitter).append(0);
}

pub fn is(r: *Rep, aspect: Aspects) bool {
    return r.stats[@intFromEnum(aspect)] != 0.0;
}

test "aspects size" {
    try testing.expectEqualDeep(35, @typeInfo(Aspects).Enum.fields.len);
}

test "rep test" {
    const i = createRep(3, true);
    try testing.expect(reps[i].stats[0] != 0);
}

test "rep size" {
    const i = createRep(3, true);
    try testing.expect(get(i, Aspects.spool) == 1);
}

// test "rep clone" {
//     const i = createRep(3, true);
//     const input: [256]u8 = undefined;
//     const str = try createRepCloneString(i, &input);
//     print("{s}", .{str});
//     try testing.expect(str.len < 0);
// }
