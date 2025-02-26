const std = @import("std");
const i_upgrade = @import("upgrade.zig");
const UpGeneral = i_upgrade.UpGeneral;

pub const Layer = struct {
    // minTimesMod: f16,
    // closedTiles: []@Vector(2, i16),
    spentLen: u32,
    ups: []UpGeneral,
};

pub var createInfos: []i_upgrade.TileVisualCreateInfo = undefined;

pub fn createCreateInfos(layer: *Layer) bool {
    const allocator = std.heap.page_allocator; // You can use any allocator or pass it as an argument.
    const count = layer.ups.len;

    if (createInfos.len > 0) {
        std.debug.print("free createInfos\n", .{});
        allocator.free(createInfos);
    }

    createInfos = allocator.alloc(i_upgrade.TileVisualCreateInfo, count)
                  catch return false;
    for (layer.ups, 0..) |up, i| {
        createInfos[i] = i_upgrade.gatherCreateInfo(&up);
        std.debug.print("ci: {any}\n", .{createInfos[i]});
    }
    
    return true;
}

pub fn findUpIndex(ups: []UpGeneral, uid: i32) i16 {
    for (ups, 0..) |value, i| {
        if (value.id == uid) return @intCast(i);
    }
    return -1;
}

pub fn findUpgrade(ups: []UpGeneral, uid: i32) !*const UpGeneral {
    for (ups) |value| {
        if (value.id == uid) return &value;
    }
    return error.CouldNotFindUpgrade;
}

pub fn findTile(ups: []UpGeneral, x: i16, y: i16, size: u8, uidIgnore: i32) i16 {
    for (ups, 0..) |tile, i| {
        if (uidIgnore != -1 and uidIgnore == tile.id) 
            continue;
        if (tile.placed and !tile.eaten
            and tile.pos[0] <= x + (size - 1) and tile.pos[0] + (tile.size - 1) >= x
            and tile.pos[1] <= y + (size - 1) and tile.pos[1] + (tile.size - 1) >= y
            ) {
            return @intCast(i);
        }
    }
    return -1;
}
	

pub fn placeTile(layer: *Layer, uid: i32, x: i16, y: i16) bool {
    const upIndex = findUpIndex(layer.ups, uid);
    if (upIndex == -1) return false;
    var up = &layer.ups[@intCast(upIndex)];

    // combine:
    const existingIndex = findTile(layer.ups, x, y, up.size, uid);
    if (existingIndex != -1) {
        var existingTile = &layer.ups[@intCast(existingIndex)];
        if (existingTile.combineType == -1
            or existingTile.combineType != up.combineType) return false;
        existingTile.combinedTiles = &[_:0]u8 {@intCast(up.id)};
        std.debug.print("found existing tile: {any}\n", .{existingTile});
        up.eaten = true;
    }

    up.pos[0] = x;
    up.pos[1] = y;
    up.placed = true;
    std.debug.print("found tile: {any}\n", .{up});
    return true;
}

pub fn regenTile(layer: *Layer, up: UpGeneral) void {
    std.debug.print("regen Tile\n", .{});

    if (layer.spentLen >= layer.ups.len) {
        const allocator = std.heap.page_allocator;
        var newUps = allocator.alloc(i_upgrade.UpGeneral, 2*layer.ups.len) catch return;
        for (layer.ups, 0..) |value, i| {
            newUps[i] = value;
        }
        allocator.free(layer.ups);
        layer.ups = newUps;
    }

    layer.ups[layer.spentLen] = i_upgrade.UpGeneral {
        .id = @intCast(layer.spentLen),
        .cost = 4,
        .lvl = 0,
        .maxLvl = 3,
        .name = up.info, 
        .info = up.info,
        .pos = @Vector(2, i16){up.pos[0], 0},
        .size = up.size,
        .imageName = up.imageName,
        .sfxName = up.sfxName,
        .uniqueTag = up.uniqueTag,
        .uniqueData = i_upgrade.UpUniqueData {
            .regular = 2,
        },
        .tooltipType = up.tooltipType,
        .combineType = up.combineType,
    };    
    layer.spentLen += 1;
}

test "resize ups" {
    const allocator = std.heap.page_allocator;
    var layer = Layer{.spentLen = 1, .ups = undefined};
    layer.ups = try allocator.alloc(i_upgrade.UpGeneral, 1);

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
        .combineType = 2,
    };
     
    regenTile(&layer, example);
    try std.testing.expect(layer.ups.len == 2);
}
