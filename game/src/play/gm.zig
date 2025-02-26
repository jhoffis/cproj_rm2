const std = @import("std");
const i_player = @import("player.zig");
const i_layer = @import("layer.zig");
const Layer = i_layer.Layer;
const i_upgrade = @import("upgrade.zig");

// Store memory of previous rounds! Important for multiplayer synchronizing.
pub const RoundInfo = struct {
    tracklength: u32,
};


pub var starting_money: i32 = 100;
pub var starting_inflation: u16 = 20;
pub var std_income: u32 = 200;
pub var max_income: u32 = 0;
pub var max_income_type: u8 = 0;
pub const max_income_types = "None\nLinear";
pub var tracklength: u32 = 245;
pub var tracklength_increase: u32 = 0;
pub var tracklength_increaser_type: u8 = 1;
pub const tracklength_increaser_names = "None\nDynamic\nLinear\nPercent";
pub var end_goal: u32 = 4;
pub var end_goal_type: u8 = 0;
pub const end_goal_names = "Ahead by points\nMost points\nFirst to points";
pub var catch_up_enabled = true;
pub var catch_up_value: u8 = 3;
pub var loser_money_type: u8 = 0;
pub var loser_money_types = "None\n+10%\n+15%";
pub var slow_time_type: u8 = 0;
pub const slow_time_types = "None\n-25%\n-33%\n-50%\n";
pub var slow_time_time_type: u8 = 0;
pub const slow_time_time_types = "5000ms\n2500ms\n1000ms\n"; // TODO make into nums


pub fn init(layer: *Layer) void {
    const allocator = std.heap.page_allocator;
    const example = i_upgrade.UpGeneral {
        .id = 0,
        .cost = 69,
        .lvl = 0,
        .maxLvl = 3,
        .name = "from gm layer",
        .info = "infooooo",
        .pos = @Vector(2, i16){3, 0},
        .size = 2,
        .imageName = i_upgrade.ImageName.cylinder,
        .sfxName = i_upgrade.SfxName.power,
        .uniqueData = i_upgrade.UpUniqueData {
            .regular = 2,
        },
        .tooltipType = i_upgrade.TooltipType.clutch,
        .combineType = -1,
    };
    layer.spentLen = 2;
    layer.ups = allocator.alloc(i_upgrade.UpGeneral, 5)
                catch return;
    layer.ups[0] = example;
    layer.ups[1] = i_upgrade.UpGeneral {
        .id = 1,
        .cost = 69,
        .lvl = 0,
        .maxLvl = 3,
        .name = "from gm layer2",
        .info = "infoooooooo!!!!\nand even more info!",
        .pos = @Vector(2, i16){1, 0},
        .imageName = i_upgrade.ImageName.piston,
        .sfxName = i_upgrade.SfxName.power,
        .uniqueData = i_upgrade.UpUniqueData {
            .regular = 2,
        },
        .tooltipType = i_upgrade.TooltipType.clutch,
        .combineType = 2,
    };
}

pub fn finish_control() void {
    for (i_player.players, 0..) |player, i| {
        if (!i_player.playersUsed[i])
            continue;
        // TODO perhaps add an error here in case racing_round is less than finished_round
        if (player.racing_round != player.finished_round)
            return;
        
        const totalIncome: i32 = @intCast(std_income);
        i_player.players[i].bank.money += totalIncome;
        i_player.players[i].bank.moneyAdded = totalIncome;
    }
}

pub fn finalize_init_player(player: *i_player.Player) void {
    player.bank.money = starting_money;
    player.bank.inflation = starting_inflation;
}

export fn setGmStdIncome(val: u32) void {
    std_income = val;
}

export fn getGmStdIncome() u32 {
    return std_income;
}

export fn setGmTracklength(val: u32) void {
    tracklength = val;
}

export fn getGmTracklength() u32 {
    return tracklength;
}

export fn setGmTracklengthIncrease(val: u32) void {
    tracklength_increase = val;
}

export fn getGmTracklengthIncrease() u32 {
    return tracklength_increase;
}

export fn setGmTracklengthIncreaserType(val: u8) void {
    tracklength_increaser_type = val;
}

export fn getGmTracklengthIncreaserType() u8 {
    return tracklength_increaser_type;
}

export fn getGmTracklengthIncreasers() [*:0]const u8 {
    return tracklength_increaser_names;
}

export fn getGmTracklengthIncreaserSuffix() [*:0]const u8 {
    return switch (tracklength_increaser_type) { // TODO use enums!!!
        2 => return " meters",
        3 => return "%",
        else => "",
    };
}

export fn setGmEndGoal(val: u32) void {
    end_goal = val;
}

export fn getGmEndGoal() u32 {
    return end_goal;
}

export fn setGmEndGoalType(val: u8) void {
    end_goal_type = val;
}

export fn getGmEndGoalType() u8 {
    return end_goal_type;
}

export fn getGmEndGoalNames() [*:0]const u8 {
    return end_goal_names;
}

export fn getGmEndGoalSuffix() [*:0]const u8 {
    if (end_goal_type == 1) { // TODO use enums!!!
        return " rounds";
    } 
    return " points";
}

export fn setGmCatchUpEnabled(val: bool) void {
    catch_up_enabled = val;
}

export fn getGmCatchUpEnabled() bool {
    return catch_up_enabled;
}

export fn setGmCatchUpValue(val: u8) void {
    catch_up_value = val;
}

export fn getGmCatchUpValue() u8 {
    return catch_up_value;
}

export fn setGmMaxIncome(val: u32) void {
    max_income = val;
}

export fn getGmMaxIncome() u32 {
    return max_income;
}

export fn setGmMaxIncomeType(val: u8) void {
    max_income_type = val;
}

export fn getGmMaxIncomeType() u8 {
    return max_income_type;
}

export fn getGmMaxIncomeTypes() [*:0]const u8 {
    return max_income_types;
}

export fn setGmStartingMoney(val: u32) void {
    starting_money = @intCast(val);
}

export fn getGmStartingMoney() u32 {
    return @intCast(starting_money);
}

export fn setGmStartingInflation(val: u32) void {
    starting_inflation = @intCast(val);
}

export fn getGmStartingInflation() u32 {
    return @intCast(starting_inflation);
}

export fn setGmExtraMoneyType(val: u8) void {
    loser_money_type = val;
}

export fn getGmExtraMoneyType() u8 {
    return loser_money_type;
}

export fn getGmExtraMoneyTypes() [*:0]const u8 {
    return loser_money_types;
}

export fn setGmSlowTimeType(val: u8) void {
    slow_time_type = val;
}

export fn getGmSlowTimeType() u8 {
    return slow_time_type;
}

export fn getGmSlowTimeTypes() [*:0]const u8 {
    return slow_time_types;
}

export fn setGmSlowTimeTimeType(val: u8) void {
    slow_time_time_type = val;
}

export fn getGmSlowTimeTimeType() u8 {
    return slow_time_time_type;
}

export fn getGmSlowTimeTimeTypes() [*:0]const u8 {
    return slow_time_time_types;
}

