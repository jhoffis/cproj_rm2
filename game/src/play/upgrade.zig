const std = @import("std");


pub const TooltipType = enum {
    none, regval, clutch, gearbox
};

pub const SfxName = enum {
    power,
};

pub const ImageName = enum {
    cylinder, piston, crank, cylinder_with_piston, 
    // These need to be last:
    open, frame
};

export fn getTileSpritesLen() i32 {
    // return @typeInfo(ImageName).@"enum".fields.len;
    return @typeInfo(ImageName).Enum.fields.len;
}

export fn getTileSprite(i: u32) [*:0]const u8 {
    // return @typeInfo(ImageName).@"enum".fields[i].name;
    return @tagName(@as(ImageName, @enumFromInt(i))).ptr;
}

// these are not names but rather differing acting upgradetiles
pub const UpUniqueTag = enum {
    regular, // cylinder, piston, gearbox, fuelcontainer
};

pub const UpUniqueData = union(UpUniqueTag) {
    regular: u8,
};

pub const UpGeneral = struct {
    id: i32,
    cost: i32,
    lvl: i32,
    maxLvl: i32,
    name: [*:0]const u8,
    info: [*:0]const u8,
    pos: @Vector(2, i16),
    size: u8 = 1,
    imageName: ImageName,
    sfxName: SfxName,
    uniqueTag: UpUniqueTag = UpUniqueTag.regular,
    uniqueData: UpUniqueData,
    tooltipType: TooltipType = TooltipType.regval,
    combineType: i32 = -1,
    combinedTiles: [*:0]const u8 = &[_:0]u8 {},
    placed: bool = false,
    open: bool = true,
    generateNew: bool = true,
    movable: bool = true,
    hasTooltip: bool = true,
    eaten: bool = false,
};

//
//  CONNECTION TO JAVA:
//
pub const TileVisualCreateInfo = extern struct {
    upTypeId: u8,
    upId: i32,
    cost: i32,
    lvl: i32,
    maxLvl: i32,
    title: [*:0]const u8,
    info: [*:0]const u8,
    image: u32,
    sfxName: u32, 
    tooltipType: i32,
    combineType: i32,
    tileSize: u8,
    logicalX: i16,
    logicalY: i16,
    placed: u8,
    open: u8,
    generateNew: u8,
    movable: u8,
    hasTooltip: u8,
    eaten: u8,
    combinedTiles: [*:0]const u8,
};

pub fn gatherCreateInfo(up: *const UpGeneral) TileVisualCreateInfo {
    return TileVisualCreateInfo {
        .upTypeId = @intFromEnum(up.uniqueTag),
        .upId = up.id,
        .cost = up.cost,
        .lvl = up.lvl,
        .maxLvl = up.maxLvl,
        .title = "asdf", // up.name,
        .info = "asdf", // up.info,
        .image = @intFromEnum(up.imageName), // TileNames.Cylinder,
        .sfxName = @intFromEnum(up.sfxName), // UpgradeSfxTypes.Power,
        .tooltipType = @intFromEnum(up.tooltipType),
        .combineType = up.combineType,
        .tileSize = up.size,
        .logicalX = up.pos[0],
        .logicalY = up.pos[1],
        .placed = @intFromBool(up.placed),
        .open = @intFromBool(up.open),
        .generateNew = @intFromBool(up.generateNew),
        .movable = @intFromBool(up.movable),
        .hasTooltip = @intFromBool(up.hasTooltip),
        .eaten = @intFromBool(up.eaten),
        .combinedTiles = up.combinedTiles,
    };
}


// public interface UpgradeGeneral extends ICloneStringable {
//
// 	UpgradeGeneral clone();
// 	boolean isOpenForUse();
// 	boolean isPlaced();
// 	byte getNameID();
// 	TileNames getTileName();
// 	UpgradeSfxTypes getSfxName();
// 	void setPremadePrice(float price);
// 	float getPremadePrice();
// 	int getCost(float sale);
// 	int getSellPrice(int round);
// //	void sell(Bank bank, Rep rep, Upgrades upgrades, int round);
// 	UpgradeType getUpgradeType();
//     void setVisible(boolean b);
// }

