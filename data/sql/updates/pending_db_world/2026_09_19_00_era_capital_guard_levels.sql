-- Era Classic (1.12) capital-city guard levels.
--
-- AzerothCore ships WotLK-inflated levels for the capital-city race guards: vanilla level 55,
-- raised to 65 in patch 2.0.1, and 75/80 by WotLK. Era runs Expansion=0, which caps player
-- content but does NOT touch stored creature_template.minlevel/maxlevel (read verbatim by
-- Creature::SelectLevel), so these guards still show level 75/80 in the level-60 Era world.
-- Restore the authentic 1.12 level 55 for the standard capital guards players interact with.
--
-- Sources: wowhead classic npc=68 (Stormwind City Guard, level 55, "raised from 55 to 65 in
-- patch 2.0.1") and npc=3296 (Orgrimmar Grunt, level 55); Honor Guard npc=3083 (vanilla 55).
UPDATE `creature_template` SET `minlevel`=55, `maxlevel`=55 WHERE `entry` IN (
    68,    -- Stormwind City Guard
    3083,  -- Honor Guard (Thunder Bluff)
    3084,  -- Bluffwatcher (Thunder Bluff)
    3296,  -- Orgrimmar Grunt
    4262,  -- Darnassus Sentinel
    5595,  -- Ironforge Guard
    5624   -- Undercity Guardian
);
