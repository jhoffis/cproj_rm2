const std = @import("std");
const testing = std.testing;
const rep = @import("rep.zig");
const Aspects = rep.Aspects;
const carstats = @import("carstats.zig");
const carfuncs = @import("carfuncs.zig");
const math = std.math;
const main = @import("main.zig");

export fn tryTireboost(repId: u8, timeDiff: u32, time: u64) bool {
    const r = &rep.reps[repId];
    const s = &carstats.tempstats;

    if (rep.is(r, Aspects.tb)) {
        s.tireboostTimeLeft = carfuncs.tireboostTimeLeft(time, 
                                                         timeDiff,
                                                         carstats.stat(s, Aspects.tbMs), 
                                                         rep.is(r, Aspects.tbArea));
        return true;
    }
    return false;
}

pub fn tireboostLoss(timeDiff: u32) u32 {
    return timeDiff - timeDiff;
}
