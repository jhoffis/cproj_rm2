
pub const RegValType = enum {
    normalPercent, additionPercent, decimal, unlock
};

pub const RegVal = struct {

    value: f64,
    type: RegValType,
    only: bool,
    unsigned: bool,
    removeAtPlacement: bool,
    replaceNeg: bool,
    ignoreDifferentNewOne: bool,

};
