const std = @import("std");
const print = std.debug.print;

const i_rep = @import("rep.zig");
const Rep = i_rep.Rep;
const i_bank = @import("bank.zig");
const Bank = i_bank.Bank;
const i_upgrade = @import("upgrade.zig");
const i_layer = @import("layer.zig");
const Layer = i_layer.Layer;
const i_gm = @import("gm.zig");

pub const Player = struct {
    name: []const u8,
    id: u8, // also used as channel
    role: u8, 
    podium: u8, // TODO should be podiums and times!
    racing_round: u8,
    finished_round: u8,
    fully_joined: bool,
    steamID: u64,
    gameID: u32,
    bank: Bank,
    rep: Rep,
    layer: Layer,
};

pub var players: [8]Player = undefined;
pub var playersUsed: [8]bool = undefined;

// TODO generalize this as it is also used in Rep
pub fn findAvailable() u8 {
    var id: u8 = 0;
    var found: bool = false;
    for (players, 0..) |_, i| {
        if (playersUsed[i]) continue;
        id = @intCast(i); 
        playersUsed[i] = true;
        found = true;
        break;
    }
    print("creating repsused: {any}\n", .{playersUsed});
    if (!found) {
        print("DIDN'T FIND AVAILABLE PLAYER SPACE!", .{});
    }
    return id;
}

pub fn initPlayer(steamId: u64, name: []const u8) u8 {
    print("init steamId: {}, name: {s}\n", .{steamId, name});
    const id = findAvailable();
    const player = &players[id];
    player.id = id;
    player.fully_joined = false;
    player.steamID = steamId;
    player.name = name;
    player.bank.money = 0;

    i_gm.init(&player.layer);

    print("initialized player: {any}\n", .{player});
    return id;
}

export fn finalize_init_players() void {
    for (players, 0..) |_, i| {
        i_gm.finalize_init_player(&players[i]);
    }
}

export fn createCreateInfos(pid: u8) void {
    _ = i_layer.createCreateInfos(&players[pid].layer);
}

export fn sizeCreateInfos(pid: u8) u32 {
    return players[pid].layer.spentLen;
}

export fn getCreateInfo(i: u32) ?*const i_upgrade.TileVisualCreateInfo {
    return &i_layer.createInfos[i];
}

export fn getMoney(pid: u8) i32 {
    return players[pid].bank.money;
}

export fn buyTile(pid: u8, uid: i32, x: i16, y: i16) bool {

    // 		if (!tile.ci.isPlaced() && tile.ci.isGenerateNew()) {
    // 			StoreAndBoardVisual.regenTile(tile);
    // 		}
    // 		
    // 		var existingTile = StoreAndBoardVisual.get(logicalX, logicalY, tile);
    // 		if (existingTile == null) {
    // 			tile.place(logicalX, logicalY, genRealPos(logicalX, logicalY));
    // 		} else if (existingTile.ci.tileSize >= tile.ci.tileSize
				// && existingTile.ci.combineType == tile.ci.combineType) {
    // 			existingTile.combine(tile);
    // 			currentTile = existingTile;
    // 			pressedTile = existingTile;
    // 			currentTile.mouseAbove = true;
    // 		} else {
    // 			return false;
    // 		}
    // 		
    // 		showUpgrades();
    const layer = players[pid].layer;
    const upgrade = i_layer.findUpgrade(layer.ups, uid) catch return false;
    const prevUp = upgrade.*;
    const cost = prevUp.cost;
    if (players[pid].bank.money < cost)
        return false;
    if (!i_layer.placeTile(&players[pid].layer, uid, x, y))
        return false;
    players[pid].bank.money -= cost;
    if (upgrade.generateNew and !prevUp.placed) {
        i_layer.regenTile(&players[pid].layer, prevUp);
    }
    return true;
}

export fn finish(pid: u8) void {
    var player = players[pid];
    player.finished_round = player.racing_round;
    i_gm.finish_control();
}

pub fn reset(player: *Player) void {
    i_rep.reset(&player.rep);
    i_bank.reset(&player.bank);
}
