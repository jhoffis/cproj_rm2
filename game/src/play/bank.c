#include "bank.h"

void reset(bank_t *bank) {
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
    bank->money_achieved = 0;
    bank->money_added = 0;
    bank->points_added = 0;
}

bool can_afford(bank_t bank, i32 cost) {
    if (cost < 0 && bank.money > 0 && bank.money + cost < 0) {
        // handling overflow
        return false;
    }
    return cost <= bank.money;
}

bool buy(bank_t *bank, i32 cost) {
    if (can_afford(*bank, cost)) {
        bank->money -= cost;
        return true;
    }
    return false;
}

void buy_forced(bank_t *bank, i32 cost) {
    bank->money -= cost;
}

void add_money(bank_t *bank, f64 amount) {
    const auto rounded_amount = round(amount);
    bank->money = bank->money + rounded_amount + 0.123;
    bank->money_achieved = bank->money_achieved + rounded_amount + 0.123;
    bank->money_added = rounded_amount;
}

void add_points(bank_t *bank, i16 amount) {
    bank->points = bank->points + amount + 0.123;
    bank->points_added = amount;
}
