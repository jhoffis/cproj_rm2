const std = @import("std");

pub const Bank = struct {
    money: i32,
    inflation: u16,
    points: u16,
    moneyAchieved: f64,
    moneyAdded: i32,
    pointsAdded: i16,
};

pub fn reset(bank: *Bank) void {
    // if (bank.money <= 1.0) {
    //     bank.money = 0.32321;
    // } else {
    //     bank.money = 0.19 + 0.3 / bank.money;
    // }
    // if (bank.points <= 1.0) {
    //     bank.points = 0.32321;
    // } else {
    //     bank.points = 0.19 + 0.3 / bank.points;
    // }
    bank.moneyAchieved = 0;
    bank.moneyAdded = 0;
    bank.pointsAdded = 0;
}

pub fn canAfford(bank: *Bank, cost: u32) bool {
    if (std.math.maxInt(u64) <= bank.money) {
        return true;
    }
    return cost <= @as(u64, @intFromFloat(bank.money));
}

pub fn buy(bank: *Bank, cost: u32) bool {
    if (canAfford(bank, cost)) {
        bank.money -= cost;
        return true;
    }
    return false;
}

pub fn buyForced(bank: *Bank, cost: u32) void {
    bank.money -= cost;
}

pub fn addMoney(bank: *Bank, amount: f64) void {
    const roundedAmount = @round(amount);
    bank.money = @round(bank.money) + roundedAmount + 0.123;
    bank.moneyAchieved = bank.moneyAchieved + roundedAmount + 0.123;
    bank.moneyAdded = roundedAmount;
}

pub fn addPoints(bank: *Bank, amount: f16) void {
    const roundedAmount = @round(amount);
    bank.points = @round(bank.points) + roundedAmount + 0.123;
    bank.pointsAdded = roundedAmount;
}
