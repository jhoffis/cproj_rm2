const std = @import("std");
const testing = std.testing;
const rep = @import("rep.zig");
const Aspects = rep.Aspects;
const carstats = @import("carstats.zig");
const math = std.math;
const main = @import("main.zig");

pub const soundBarrierSpeed = 1234;

pub fn tireboostLoss(tbArea: bool, timeDiff: u64) f64 {
    if (tbArea)
        return 1;

    var timeloss = 1 - (@as(f64, @floatFromInt(timeDiff)) * 0.001);
    if (timeloss < 0)
        timeloss = 0;
    return timeloss;
}

pub fn tireboostTimeLeft(now: u64, timeDiff: u64, tbMs: f64, tbArea: bool) u64 {
    return now + @as(u64, @intFromFloat(tbMs * tireboostLoss(tbArea, timeDiff))) - timeDiff;
}

pub fn turboHp(r: *rep.Rep,
               spool: f64, 
               turboblow: f64,
               turboblowStrength: f64,
               blowON: bool,
               throttle: bool) f64 {
    var thp = rep.turboHP(r) * spool;
    if (blowON and throttle and turboblow >= 0) {
        const factor = turboFactor(turboblow);
        thp *= factor + turboblowStrength;
    }
    return thp;
}

export fn turboHorsepower(repId: u8,
                          spool: f64, 
                          turboblow: f64,
                          turboblowStrength: f64,
                          blowON: bool,
                          throttle: bool) f64 {
    return turboHp(&rep.reps[repId], spool, turboblow, turboblowStrength, blowON, throttle);
}

export fn spendTurboblow(turboblow: f64, tickFactor: f64) f64 {
    const factor = turboFactor(turboblow);
    return tickFactor * 2.0*factor;
}

export fn rpmToPercent(rpm: f64, idle: f64, top: f64) f64 {
    const baseMin = idle * 0.5;
    if (rpm < baseMin)
        return baseMin / top;    
    return rpm / top;
}

export fn calcGearDrag(gear: u32, gearTop: f64) f64 {
    const gearCmpTop = @as(f64, @floatFromInt(gear - 1)) / gearTop;
    var drag = 1.0 - (0.12 * gearCmpTop); // TODO gjør ekte kalkulering på hva forskjellen mellom de forskjellige gir egt er ift dreiemoment og fart

    if (drag < 0.1)
        drag = 0.1;
    return drag;
}

export fn gearMax(spdTop: f64, gear: u32, gearTop: f64) f64 {
    return @as(f64, @floatFromInt(gear)) * (spdTop / gearTop);
}

export fn gearMaxId(repId: u8, gear: u32) f64 {
    const r = &rep.reps[repId];
    const spdTop = rep.getStat(r, Aspects.spdTop);
    const gearTop = rep.getStat(r, Aspects.gearTop);
    return gearMax(spdTop, gear, gearTop);
}

export fn isGearCorrect(speed: f64, spdTop: f64, gear: u32, gearTop: f64) bool {
    const gearmax = gearMax(spdTop, gear, gearTop);
    return speed < gearmax;
}

export fn decelerateCar(rpm: f64, idle: f64, top: f64, gear: u32, speed: f64, kg: f64, aero: f64, brake: f64, clutch: bool) f64 {

    var rpmPerc = rpmToPercent(rpm, idle, top);
    if (rpmPerc > 1)
        rpmPerc = 1;
    var dec: f64 = -1.0;
    if (clutch or gear == 0) {
        dec *= 0.5;
    } else {
        dec *= rpmPerc;
    }
    var extraDec = 0.5 * math.pow(f64, 3, 0.0025 * speed);
    if (@abs(extraDec) > 250)
        extraDec = 250;
    dec *= extraDec;

    //TODO mer aggressiv og realistisk vindmotstand IKKE VINDLØSHET!
    //		System.out.println("drag: " + drag + ", air: " + Math.pow(speed, 24d));

    var windless: f64 = 100.0 / speed + (10 * 100 / speed);

    // lose more with lower weight
    const kgWindless = weightWindless(kg);
    windless *= kgWindless;
    //		System.out.println(weightWindless);
    //		var power = Math.pow(stats.stats[Rep.kW] / 300d, 2) * (rpmPerc > 1 ? 1 : rpmPerc);
    //		System.out.println("Power: " + power);
    //		if (power < 1)
    //			power = 1;
    //		else if (power > 5)
    //			power = 5;
    //		windless /= power;
    //		System.out.println("Windless: " + windless);

    if (windless < 1)
        windless = 1;
    //		else if (windless > 1.5d)
    //			windless = 1.5d;
    dec /= windless;

    //		System.out.println("dec: " + dec);
    // if (rep.is(Rep.snos) and stats.NOSON and stats.throttle) {
    //     var snosDec = stats.stats[Rep.nos];
    //     dec = (1d + (-6d*dec)) * snosDec;
    // } else {
    dec *= aero;
    dec *= 1 + 5 * brake / (0.000001 + aero);
    dec -= 2.5 * brake;
    // }

    return dec;
}

export fn calculateDistance(speed: f64, tickFactor: f64) f64 {
    // 25 ticks per sec. kmh, distance in meters. So, x / 3.6 / 25.
    // if tickFactor == 1 then that is 1 tick which is 1/25sec.
    return (speed / 3.6) / main.TICK * tickFactor;
}

pub fn speedInc(s: *carstats.Stats, r: *rep.Rep, tickFactor: f64, comparedTimeLeft: u64) f64 {
    const w = carstats.stat(s, Aspects.kg);
    var rpmCalc: f64 = 1.0;
    const gear = s.gear;
    if (!rep.is(r, Aspects.stickyclutch)) {
        rpmCalc = rpmToPercent(s.rpm, carstats.stat(s, Aspects.rpmBaseIdle), carstats.stat(s, Aspects.rpmBaseTop));
        if (gear == 1) { // Litt ekstra push til starten
            rpmCalc += 0.2 * (1.0 - rpmCalc);
        }
    } else {
        rpmCalc = rpmToPercent(carstats.stat(s, Aspects.rpmTop), carstats.stat(s, Aspects.rpmBaseIdle), carstats.stat(s, Aspects.rpmBaseTop));
    }

    var hp = carstats.stat(s, Aspects.hp);

    if (carstats.hasTurbo(s)) {
        const turboblow = carstats.stat(s, Aspects.turboblow);
        const thp = turboHp(r, s.spool, turboblow, carstats.stat(s, Aspects.turboblowStrength), s.turboBlowON, s.throttle);

        if (s.turboBlowON and s.throttle and turboblow >= 0) {
            s.stats[@intFromEnum(Aspects.turboblow)] -= spendTurboblow(carstats.stat(s, Aspects.turboblow), tickFactor);
        }

        hp += thp;
    } else {
        s.turboBlowON = false;
    }

    const tothp = hp * rpmCalc / w;

    const speedTop = carstats.stat(s, Aspects.spdTop);
    const gearTop = carstats.stat(s, Aspects.gearTop);
    const basetopSpd = carstats.stat(s, Aspects.spdTopBase);
    var bobatea = basetopSpd / speedTop;
    if (gear <= 0) {
        bobatea = 1;
    } else {
        if (bobatea < 0.01)
            bobatea = 0.01;
        bobatea *= gearTop / @as(f64, @floatFromInt(gear));
    }
    
    var spdInc = tothp * calcGearDrag(gear, gearTop) * bobatea;
    //		System.out.println((gear > 0 ? bobatea * (gearTop / gear) : 1));

    const minRpmCalc = 0.2;
    rpmCalc = rpmCalc * (1.0 - minRpmCalc) + minRpmCalc; // ikke nerf boost for mye basert p� rpm. Dytter tallene
                                                        // n�rmere 1.0

    if (rep.is(r, Aspects.nos)) {
        var nosAmount: f64 = 0;
        for (@intFromEnum(Aspects.nosBottles), 0..) |_, i| {
            if (carstats.getNosTimeLeft(s, i) > comparedTimeLeft) {
                nosAmount += 1;
                if (!rep.is(r, Aspects.snos))
                    spdInc += carstats.stat(s, Aspects.nos) / nosAmount * rpmCalc;
            }
        }
        s.NOSON = nosAmount > 0;
    }

    if (s.tireboostTimeLeft > comparedTimeLeft) {
        spdInc += rep.getStat(r, Aspects.tb) * rpmCalc;
        s.tireboostON = true;
    } else {
        s.tireboostON = false;
    }

    // if (math..isInfinite(spdInc) || Double.isNaN(spdInc)) {
    //     spdInc = Double.MAX_VALUE;
    // }

    return spdInc;
}

pub fn idleRpmRandomizer(rpm: f64, hp: f64) f64 {
    var percentageShake = hp / 20000.0;
    if (percentageShake < 0.03) {
        percentageShake = 0.03;
    } else if (percentageShake > 0.75) {
        percentageShake = 0.75;
    }
    var res = rpm * percentageShake;
    res -= res / 2.0;
    return rpm + res;
}

// Updates RPM value based on engaged clutch and throttle and current speed
pub fn updateRPM(s: *carstats.Stats, r: *rep.Rep, tickFactor: f64, timeNow: u64) void {
    var rpmChange: f64 = 0;
    var rpm: f64 = s.rpmGoal;
    const rpmTop = carstats.stat(s, Aspects.rpmTop);
    const rpmIdle = carstats.stat(s, Aspects.rpmIdle);
    //		if (rpm < 0)
    //			System.out.println("rpm: " + rpm);

    if (s.clutchPercent < 1) {
        //			System.out.println("clutch engaged");
        // If clutch engaged
        const gearTop = carstats.stat(s, Aspects.gearTop);
        var change = rpm; // push prev rpm 
        const gearFactor = s.speed / (gearMax(carstats.stat(s, Aspects.spdTop), s.gear, gearTop) + 1);
        rpm = carstats.stat(s, Aspects.rpmTop) * gearFactor;

        // Turbo spooling
        change = rpm - change; // pop and calc diff of (next - prev) rpm
        if (carstats.hasTurbo(s)) {
            var increase = 0.0000045 * carstats.stat(s, Aspects.spool);
            if (s.spool > 1)
                increase *= 0.04;
            s.spool += rpm * increase * tickFactor;
        }
        //			else {
        //				stats.spool = 0;
        //			}

        var minimum: f64 = 0;
        const totalHP = rep.totalHP(r);

        if (s.redlinedThisGear) {
            rpm = idleRpmRandomizer(rpm, totalHP);
        } else {
            if (s.throttle) {
                minimum = idleRpmRandomizer(rpmIdle * 2 / 3, totalHP);
            } else {
                minimum = idleRpmRandomizer(rpmIdle, totalHP);
            }

            if (rpm < minimum)
                rpm = minimum;
        }

        if (s.grindingTime == 0 or (s.redlinedThisGear and s.gear != @as(u32, @intFromFloat(gearTop)))) {
            s.grindingTime = timeNow;
        }

    } else if (s.throttle) { 
        //			System.out.println("not clutch engaged " + stats.clutchPercent);
        // Not engaged clutch
        var maxRpm: f64 = 0;
        var redRpm: f64 = 0;

        if (carstats.hasTwoStep(s)) {
            maxRpm = 0.75 * rpmTop; 
            redRpm = 0.75 * rpmTop - 0.2*rpmTop;
        } else {
            maxRpm = rpmTop - 60; 
            redRpm = rpmTop - 100; 
        }

        if (s.rpm < maxRpm) {
            const rpmFactor: f64 = (rpmTop * 0.8) + (rpm * 0.2);
            rpmChange = carstats.stat(s, Aspects.hp) * (rpmFactor / rpmTop) * s.throttlePercent;
            s.spool = carstats.stat(s, Aspects.spoolStart);
        } else {
            // Redlining
            rpm = redRpm;
        }
    } else {
        //			System.out.println("not clutch engaged and not throttle");
        // Not engaged and throttle not down
        if (rpm > rpmIdle) {
            rpmChange = -(carstats.stat(s, Aspects.hp) * 0.25);

            const maxLoss = rpmTop / 32;
            if (rpmChange < -maxLoss)
                rpmChange = -maxLoss;
        } else
            // Sets RPM to for instance 1000 rpm as standard.
            rpm = idleRpmRandomizer(rpmIdle, rep.totalHP(r));
        s.spool = carstats.stat(s, Aspects.spoolStart);
    }

    s.rpmGoal = rpm + rpmChange * tickFactor;

    const min = rpmIdle / 2.0;
    if (s.rpmGoal < min)
        s.rpmGoal = min;

    const diff: f64 = s.rpmGoal - s.rpm;
    if (@abs(diff) > 100.0) {
        s.rpm = s.rpm + (diff * tickFactor);
    } else {
        s.rpm = s.rpmGoal;
    }
}

export fn updateSpeed(repId: u8, tickFactor: f64, time: u64) void {
    var speedChange: f64 = 0;
    const s = &carstats.tempstats;
    const r = &rep.reps[repId];
    const rpmBaseIdle = carstats.stat(s, Aspects.rpmBaseIdle);
    const rpmBaseTop = carstats.stat(s, Aspects.rpmBaseTop);
    const spdTop = carstats.stat(s, Aspects.spdTop);
    const gearTop = rep.getStat(r, Aspects.gearTop);
    const dec = decelerateCar(s.rpm, rpmBaseIdle, rpmBaseTop, s.gear, s.speed, rep.getStat(r, Aspects.kg), carstats.stat(s, Aspects.aero), s.brake, s.clutch);

    // MOVEMENT
    if (s.throttle
        and !s.grinding 
        and s.clutchPercent < 1.0 
        and isGearCorrect(s.speed, spdTop, s.gear, gearTop)) {
        speedChange = speedInc(s, r, tickFactor, time) * s.throttlePercent + 0.1 * dec;
    } else {
        s.NOSON = false;
        s.tireboostON = false;
        speedChange = dec;
        if (s.grinding)
            speedChange += dec;
    }

    //		var grindPerc = (time - stats.grindingTime) / 1500f;
    //		if (grindPerc < 1f && grindPerc > 0f) {
    //			if (grindPerc < 0.5)
    //				stats.grindingCurrent = .3;
    //			else
    //				stats.grindingCurrent = 1f - (0.7 * (1f - grindPerc));
    //			if (speedChange > 0f)
    //				speedChange *= stats.grindingCurrent;
    //		} else {
    //			stats.grindingCurrent = 1;
    //		}


    // RPM
    s.spdinc = speedChange * tickFactor;

    var topspd: f64 = 0;
    if (s.gear != 0) {
        topspd = gearMax(spdTop, s.gear, gearTop);
    } else {
        topspd = spdTop;
    }
    //		System.out.println("speed inc before: " + stats.spdinc);
    if (s.speed + s.spdinc > topspd) { // Wanted to go too fast
        s.redlinedThisGear = true;
        s.spdinc = topspd - s.speed;
    } else {
        s.redlinedThisGear = false;
    }
    updateRPM(s, r, tickFactor, time);

    //		var res = stats.speed < soundBarrierSpeed && stats.speed + stats.spdinc >= soundBarrierSpeed;

    //		System.out.println(stats.spdinc);
    s.speed += s.spdinc;
    if (s.speed < 0) // FIXME || Double.isNaN(stats.speed))
        s.speed = 0;
    //		System.out.println("speed inc after: " + stats.spdinc);
    //		System.out.println("speed: " + stats.speed);

    rep.updateHighestSpeed(r, s.speed);
    s.distance += calculateDistance(s.speed, tickFactor);
}

// export fn getTBPercentageLeft() f64 {
//     float tbMaxTime = (float) ZigLib.INSTANCE.getStat(Rep.tbMs);
//     //		System.out.println("tbMaxTime " + tbMaxTime);
//     float tbTime = (float) (ZigLib.INSTANCE.getTireboostTimeLeft() - System.currentTimeMillis());
//     System.out.println("tbTime " + tbTime + ", now: " + System.currentTimeMillis());
//     var percent = tbTime / tbMaxTime;
//     //		System.out.println("Percent: " + percent);
//
//     return Math.max(percent, 0);
// }

pub fn turboFactor(turboblow: f64) f64 {
    var factor = (turboblow - 1.0) / 100.0;
    if (factor < 0)
        factor = 0;
    factor = @floor(factor) + 1.0;
    return factor;
}

pub fn weightWindless(kg: f64) f64 {
    return @max(kg / 1200, 0.5);
}

test "windless" {
    try testing.expect(weightWindless(1200) == 1);
    std.debug.print("{}\n", .{weightWindless(1)});
    try testing.expect(weightWindless(1) == 0.5);
}
