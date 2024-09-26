local cursor = "0"
local keys = {}

repeat
    -- Use SCAN to get the next batch of keys
    local res = redis.call("SCAN", cursor)
    cursor = res[1]  -- Update the cursor

    -- Iterate over the keys returned and add them to the keys list
    for _, key in ipairs(res[2]) do
        table.insert(keys, key)
    end
until cursor == "0"  -- Continue until cursor is 0

-- Return the list of keys
return keys
