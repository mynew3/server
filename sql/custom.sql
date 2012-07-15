-- Fix Vampiric Touch spell bonus data

UPDATE spell_bonus_data SET dot_bonus = "0.2" WHERE entry = 34914;
