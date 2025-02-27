#include "carfuncs.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "rep.h"
#include "car.h"
#include "text.h"
#include "timer_util.h"

const f64 sound_barrier_speed = 1234;

f64 tireboost_loss(bool tb_area, u64 time_diff) {
    if (tb_area)
        return 1;

    f64 timeloss = 1 - ((f64)time_diff * 0.001);
    if (timeloss < 0)
        timeloss = 0;
    return timeloss;
}

u64 tireboost_time_left(u64 now, u64 time_diff, f64 tb_ms, bool tb_area) {
    return now + (u64)(tb_ms * tireboost_loss(tb_area, time_diff)) - time_diff;
}

f64 turbo_factor(f64 turboblow) {
    f64 factor = (turboblow - 1.0) / 100.0;
    if (factor < 0)
        factor = 0;
    factor = floor(factor) + 1.0;
    return factor;
}

f64 weight_windless(f64 kg) {
    return fmax(kg / 1200, 0.5);
}

// f64 turbo_hp(rep_t *r,
//              f64 spool, 
//              f64 turboblow,
//              f64 turboblow_strength,
//              bool blow_on,
//              bool throttle) {
//     f64 thp = turbo_hp(r) * spool;
//     if (blow_on && throttle && turboblow >= 0) {
//         const f64 factor = turbo_factor(turboblow);
//         thp *= factor + turboblow_strength;
//     }
//     return thp;
// }

// f64 turbo_horsepower(u8 rep_id,
//                      f64 spool, 
//                      f64 turboblow,
//                      f64 turboblow_strength,
//                      bool blow_on,
//                      bool throttle) {
//     return turbo_hp(&reps[rep_id], spool, turboblow, turboblow_strength, blow_on, throttle);
// }

f64 spend_turboblow(f64 turboblow, f64 tick_factor) {
    const f64 factor = turbo_factor(turboblow);
    return tick_factor * 2.0 * factor;
}

f64 rpm_to_percent(f64 rpm, f64 idle, f64 top) {
    const f64 base_min = idle * 0.5;
    if (rpm < base_min)
        return base_min / top;    
    return rpm / top;
}

f64 calc_gear_drag(u32 gear, f64 gear_top) {
    const f64 gear_cmp_top = (f64)(gear - 1) / gear_top;
    f64 drag = 1.0 - (0.12 * gear_cmp_top); // TODO gjør ekte kalkulering på hva forskjellen mellom de forskjellige gir egt er ift dreiemoment og fart

    if (drag < 0.1)
        drag = 0.1;
    return drag;
}

f64 gear_max(f64 spd_top, u32 gear, f64 gear_top) {
    return (f64)gear * (spd_top / gear_top);
}

f64 gear_max_id(u8 rep_id, u32 gear) {
    const rep_t *r = &reps[rep_id];
    const f64 spd_top = r->stats[rep_spd_top];
    const f64 gear_top = r->stats[rep_gear_top];
    return gear_max(spd_top, gear, gear_top);
}

bool is_gear_correct(f64 speed, f64 spd_top, u32 gear, f64 gear_top) {
    const f64 gearmax = gear_max(spd_top, gear, gear_top);
    return speed < gearmax;
}

f64 decelerate_car(f64 rpm, f64 idle, f64 top, u32 gear, f64 speed, f64 kg, f64 aero, f64 brake, bool clutch) {
    f64 rpm_perc = rpm_to_percent(rpm, idle, top);
    if (rpm_perc > 1)
        rpm_perc = 1;
    f64 dec = -1.0;
    if (clutch || gear == 0) {
        dec *= 0.5;
    } else {
        dec *= rpm_perc;
    }
    f64 extra_dec = 0.5 * pow(3, 0.0025 * speed);
    if (fabs(extra_dec) > 250)
        extra_dec = 250;
    dec *= extra_dec;

    // TODO mer aggressiv og realistisk vindmotstand IKKE VINDLØSHET!
    // System.out.println("drag: " + drag + ", air: " + Math.pow(speed, 24d));

    f64 windless = 100.0 / speed + (10 * 100 / speed);

    // lose more with lower weight
    const f64 kg_windless = weight_windless(kg);
    windless *= kg_windless;
    // System.out.println(weight_windless);
    // var power = Math.pow(stats.stats[Rep.kW] / 300d, 2) * (rpm_perc > 1 ? 1 : rpm_perc);
    // System.out.println("Power: " + power);
    // if (power < 1)
    //     power = 1;
    // else if (power > 5)
    //     power = 5;
    // windless /= power;
    // System.out.println("Windless: " + windless);

    if (windless < 1)
        windless = 1;
    // else if (windless > 1.5d)
    //     windless = 1.5d;
    dec /= windless;

    // System.out.println("dec: " + dec);
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

f64 calculate_distance(f64 speed, f64 tick_factor) {
    // 25 ticks per sec. kmh, distance in meters. So, x / 3.6 / 25.
    // if tick_factor == 1 then that is 1 tick which is 1/25sec.
    return (speed / 3.6) / TIMER_TICK * tick_factor;
}

static f64 speed_inc(car_stats_t *s, rep_t *r, f64 tick_factor, u64 compared_time_left) {
    const f64 w = s->stats[rep_kg];
    f64 rpm_calc = 1.0;
    const u32 gear = s->gear;
    rpm_calc = rpm_to_percent(s->rpm, s->stats[rep_rpm_base_idle], s->stats[rep_rpm_base_top]);
    if (gear == 1) { // FIXME Litt ekstra push til starten
        rpm_calc += 0.2 * (1.0 - rpm_calc);
    }

    f64 hp = s->stats[rep_hp];

    if (has_turbo(s)) {
        const f64 turboblow = s->stats[rep_turbo];
        // const f64 thp = turbo_hp(r, s->spool, turboblow, stat(s, aspects.turboblow_strength), s->turbo_blow_on, s->throttle);

        // if (s->turbo_blow_on && s->throttle && turboblow >= 0) {
        //     s->stats[aspects.turboblow] -= spend_turboblow(stat(s, aspects.turboblow), tick_factor);
        // }
        //
        // hp += thp;
    } else {
        s->turbo_blow_on = false;
    }

    render_print("hp: %f", hp * rpm_calc);
    const f64 tothp = hp * rpm_calc / w;

    const f64 speed_top   = s->stats[rep_spd_top];
    const f64 gear_top    = s->stats[rep_gear_top];
    const f64 basetop_spd = s->stats[rep_spd_base];
    f64 bobatea = basetop_spd / speed_top;
    if (gear <= 0) {
        bobatea = 1;
    } else {
        if (bobatea < 0.01)
            bobatea = 0.01;
        bobatea *= gear_top / (f64)gear;
    }
    
    f64 spd_inc = tothp * calc_gear_drag(gear, gear_top) * bobatea;
    // System.out.println((gear > 0 ? bobatea * (gear_top / gear) : 1));

    const f64 min_rpm_calc = 0.2;
    rpm_calc = rpm_calc * (1.0 - min_rpm_calc) + min_rpm_calc; // ikke nerf boost for mye basert p� rpm. Dytter tallene
                                                        // n�rmere 1.0

    if (r->stats[rep_nos]) {
        f64 nos_amount = 0;
        for (int i = 0; i < rep_nos_bottles; i++) {
            if (get_nos_time_left(s, i) > compared_time_left) {
                nos_amount += 1;
                // if (!rsnos))
                //     spd_inc += stat(s, aspects.nos) / nos_amount * rpm_calc;
            }
        }
        s->nos_on = nos_amount > 0;
    }

    if (s->tireboost_time_left > compared_time_left) {
        spd_inc += r->stats[rep_tb] * rpm_calc;
        s->tireboost_on = true;
    } else {
        s->tireboost_on = false;
    }

    // if (isInfinite(spd_inc) || isNaN(spd_inc)) {
    //     spd_inc = DOUBLE_MAX;
    // }

    return spd_inc;
}

f64 idle_rpm_randomizer(f64 rpm, f64 hp) {
    f64 percentage_shake = hp / 20000.0;
    if (percentage_shake < 0.03) {
        percentage_shake = 0.03;
    } else if (percentage_shake > 0.75) {
        percentage_shake = 0.75;
    }
    f64 res = rpm * percentage_shake;
    res -= res / 2.0;
    return rpm + res;
}

// Updates RPM value based on engaged clutch and throttle and current speed
void update_rpm(car_stats_t *s, rep_t *r, f64 tick_factor, u64 time_now) {
    f64 rpm_change = 0;
    f64 rpm = s->rpm_goal;
    const f64 rpm_top = s->stats[rep_rpm_top];
    const f64 rpm_idle = s->stats[rep_rpm_idle];
    // if (rpm < 0)
    //     System.out.println("rpm: " + rpm);

    if (s->clutch_percent < 1) {
        // System.out.println("clutch engaged");
        // If clutch engaged
        const f64 gear_top = s->stats[rep_gear_top];
        f64 change = rpm; // push prev rpm 
        const f64 gear_factor = s->speed / (gear_max(s->stats[rep_spd_top], s->gear, gear_top) + 1);
        rpm = s->stats[rep_rpm_top] * gear_factor;

        // Turbo spooling
        change = rpm - change; // pop and calc diff of (next - prev) rpm
        if (has_turbo(s)) {
            f64 increase = 0.0000045 * s->stats[rep_spool];
            if (s->spool > 1)
                increase *= 0.04;
            s->spool += rpm * increase * tick_factor;
        }
        // else {
        //     stats.spool = 0;
        // }

        f64 minimum = 0;
        const f64 total_hp = get_total_hp(r);

        if (s->redlined_this_gear) {
            rpm = idle_rpm_randomizer(rpm, total_hp);
        } else {
            if (s->throttle) {
                minimum = idle_rpm_randomizer(rpm_idle * 2 / 3, total_hp);
            } else {
                minimum = idle_rpm_randomizer(rpm_idle, total_hp);
            }

            if (rpm < minimum)
                rpm = minimum;
        }

        if (s->grinding_time == 0 || (s->redlined_this_gear && s->gear != (u32)gear_top)) {
            s->grinding_time = time_now;
        }

    } else if (s->throttle) { 
        // System.out.println("not clutch engaged " + stats.clutch_percent);
        // Not engaged clutch
        f64 max_rpm = 0;
        f64 red_rpm = 0;

        if (has_two_step(s)) {
            max_rpm = 0.75 * rpm_top; 
            red_rpm = 0.75 * rpm_top - 0.2 * rpm_top;
        } else {
            max_rpm = rpm_top - 60; 
            red_rpm = rpm_top - 100; 
        }

        if (s->rpm < max_rpm) {
            const f64 rpm_factor = (rpm_top * 0.8) + (rpm * 0.2);
            rpm_change = s->stats[rep_hp] * (rpm_factor / rpm_top) * s->throttle_percent;
            s->spool = s->stats[rep_spool_start];
        } else {
            // Redlining
            rpm = red_rpm;
        }
    } else {
        // System.out.println("not clutch engaged and not throttle");
        // Not engaged and throttle not down
        if (rpm > rpm_idle) {
            rpm_change = -(s->stats[rep_hp] * 0.25);

            const f64 max_loss = rpm_top / 32;
            if (rpm_change < -max_loss)
                rpm_change = -max_loss;
        } else
            // Sets RPM to for instance 1000 rpm as standard.
            rpm = idle_rpm_randomizer(rpm_idle, get_total_hp(r));
        s->spool = s->stats[rep_spool_start];
    }

    s->rpm_goal = rpm + rpm_change * tick_factor;

    const f64 min = rpm_idle / 2.0;
    if (s->rpm_goal < min)
        s->rpm_goal = min;

    const f64 diff = s->rpm_goal - s->rpm;
    if (fabs(diff) > 100.0) {
        s->rpm = s->rpm + (diff * tick_factor);
    } else {
        s->rpm = s->rpm_goal;
    }
}

void update_speed(u8 rep_id, f64 tick_factor, u64 time) {
    f64 speed_change = 0;
    car_stats_t *s = &my_car;
    rep_t *r = &reps[rep_id];
    const f64 rpm_base_idle = s->stats[rep_rpm_base_idle];
    const f64 rpm_base_top = s->stats[rep_rpm_base_top];
    const f64 spd_top = s->stats[rep_spd_top];
    const f64 gear_top = r->stats[rep_gear_top];
    const f64 dec = 20 * decelerate_car(s->rpm, rpm_base_idle, rpm_base_top, s->gear, s->speed, s->stats[rep_kg], s->stats[rep_aero], s->brake, s->clutch);

    // MOVEMENT
    if (s->throttle
        && !s->grinding 
        && s->clutch_percent < 1.0 
        && is_gear_correct(s->speed, spd_top, s->gear, gear_top)) {
        speed_change = speed_inc(s, r, tick_factor, time) + dec;
    } else {
        s->nos_on = false;
        s->tireboost_on = false;
        speed_change = dec;
        if (s->grinding)
            speed_change += dec;
    }

    // var grind_perc = (time - stats.grinding_time) / 1500f;
    // if (grind_perc < 1f && grind_perc > 0f) {
    //     if (grind_perc < 0.5)
    //         stats.grinding_current = .3;
    //     else
    //         stats.grinding_current = 1f - (0.7 * (1f - grind_perc));
    //     if (speed_change > 0f)
    //         speed_change *= stats.grinding_current;
    // } else {
    //     stats.grinding_current = 1;
    // }

    // RPM
    s->spdinc = speed_change * tick_factor;

    f64 topspd = 0;
    if (s->gear != 0) {
        topspd = gear_max(spd_top, s->gear, gear_top);
    } else {
        topspd = spd_top;
    }
    // System.out.println("speed inc before: " + stats.spdinc);
    if (s->speed + s->spdinc > topspd) { // Wanted to go too fast
        s->redlined_this_gear = true;
        s->spdinc = topspd - s->speed;
    } else {
        s->redlined_this_gear = false;
    }
    update_rpm(s, r, tick_factor, time);

    // var res = stats.speed < sound_barrier_speed && stats.speed + stats.spdinc >= sound_barrier_speed;

    // System.out.println(stats.spdinc);
    s->speed += s->spdinc;
    if (s->speed < 0) // FIXME || isNaN(stats.speed))
        s->speed = 0;
    // System.out.println("speed inc after: " + stats.spdinc);
    // System.out.println("speed: " + stats.speed);

    update_highest_speed(r, s->speed);
    s->distance += calculate_distance(s->speed, tick_factor);
}

// f64 get_tb_percentage_left() {
//     float tb_max_time = (float) ZigLib.INSTANCE.get_stat(Rep.tb_ms);
//     // System.out.println("tb_max_time " + tb_max_time);
//     float tb_time = (float) (ZigLib.INSTANCE.get_tireboost_time_left() - System.currentTimeMillis());
//     System.out.println("tb_time " + tb_time + ", now: " + System.currentTimeMillis());
//     var percent = tb_time / tb_max_time;
//     // System.out.println("Percent: " + percent);
//
//     return fmax(percent, 0);
// }
