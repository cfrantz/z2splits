local running = true;
local number = 1;
local started = false;
local text = "nothing";
local tick = emu.framecount();

function reset_state()
    local state = {};
    state["crystals"] = 0;
    state["heart"] = 4;
    state["magic"] = 4;

    state["shield"] = 0;
    state["jump"] = 0;
    state["life"] = 0;
    state["fairy"] = 0;
    state["fire"] = 0;
    state["reflect"] = 0;
    state["spell"] = 0;
    state["thunder"] = 0;

    state["candle"] = 0;
    state["glove"] = 0;
    state["raft"] = 0;
    state["boots"] = 0;
    state["flute"] = 0;
    state["cross"] = 0;
    state["hammer"] = 0;
    state["magickey"] = 0;

    return state;
end

local state = reset_state();

remote = io.open("/tmp/z2splits", "w");
io.output(remote);

while (running) do
    local frame = {};

    frame["crystals"] = memory.readbyte(0x794);
    frame["heart"] = memory.readbyte(0x784)
    frame["magic"] = memory.readbyte(0x783)

    frame["shield"] =  memory.readbyte(0x77b);
    frame["jump"] =    memory.readbyte(0x77c);
    frame["life"] =    memory.readbyte(0x77d);
    frame["fairy"] =   memory.readbyte(0x77e);
    frame["fire"] =    memory.readbyte(0x77f);
    frame["reflect"] = memory.readbyte(0x780);
    frame["spell"] =   memory.readbyte(0x781);
    frame["thunder"] = memory.readbyte(0x782);

    frame["candle"] =   memory.readbyte(0x785);
    frame["glove"] =    memory.readbyte(0x786);
    frame["raft"] =     memory.readbyte(0x787);
    frame["boots"] =    memory.readbyte(0x788);
    frame["flute"] =    memory.readbyte(0x789);
    frame["cross"] =    memory.readbyte(0x78a);
    frame["hammer"] =   memory.readbyte(0x78b);
    frame["magickey"] = memory.readbyte(0x78c);

    if tick > 10 then
        remote = io.open("/tmp/z2splits", "w");
        io.output(remote);
        for k, v in pairs(frame) do
            if state[k] ~= v then
                local old = state[k];
                state[k] = v;
                if k == "crystals" then
                    if old == 0 and v == 6 then
                        k = "start";
                        started = true;
                    else
                        k = "palace " .. (6-v);
                    end
                elseif k == "heart" or k == "magic" then
                    if (v-4) > 0 then
                        k = k .. " " .. (v-4);
                    else
                        k = ""
                    end
                end
                if started and k ~= "" then
                    if k == "start" then
                        text = "command: \"start\" frame: " .. tick;
                    else
                        text = "command: \"split\" frame: " .. tick ..
                               " data: \"" .. k .. "\"";
                    end
                    io.write(text .. "\n")
                    number = number + 1;
                end
            end
        end
        io.close(remote);
    end

    gui.text(8, 8, text);
    FCEU.frameadvance();
    tick = emu.framecount();

    -- gui.text(160, 8, "Frame: " .. tick);
    if tick == 0 then
        print("Lua detected reset\n");
        number = 1;
        started = false;
        text = "nothing";
        state = reset_state();
        remote = io.open("/tmp/z2splits", "w");
        io.output(remote);
        io.write("command: \"reset\"\n")
        io.close(remote);
    end
end
