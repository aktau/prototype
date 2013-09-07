-- load the ffi library
-- local ffi = require("ffi")

-- ffi.cdef[[
-- void Sleep(int ms);
-- int poll(struct pollfd *fds, unsigned long nfds, int timeout);
-- ]]

-- local sleep
-- if ffi.os == "Windows" then
--   function sleep(s)
--     ffi.C.Sleep(s*1000)
--   end
-- else
--   function sleep(s)
--     ffi.C.poll(nil, 0, s*1000)
--   end
-- end

print("lua interpreter compatibility version: " .. tostring(_VERSION))
print("jit status: " .. tostring(jit.status()))
-- io.write("luajit ffi: ")
-- io.flush()
-- sleep(0.01)
-- print("works");


-- defines a factorial function
-- function fact (n)
--     if n == 0 then
--         return 1
--     else
--         return n * fact(n-1)
--     end
-- end

-- local fn = 5
-- local moar = 10
-- print("The factorial of " .. fn .. " is " .. fact(fn))
-- print("The factorial of " .. moar .. " is " .. fact(moar))

-- local clock = os.clock
-- local start = clock()

-- local text = {"The", "quick", "brown", "fox", "jumped", "over", "the", "lazy", "dog", "at", "a", "restaurant", "near", "the", "lake", "of", "a", "new", "era"}
-- local map = {}
-- local times = 1000000

-- while times > 0 do
--     for i = 1, table.getn(text), 1 do
--         if map[text[i]] then map[text[i]] = map[text[i]] + 1
--         else map[text[i]] = 1 end
--     end
--     times = times - 1;
-- end

-- for key, value in pairs(map) do
--     io.write(key, " ", value, "\n")
-- end

-- local stop = clock() - start

-- print("Took", 1000.0 * stop, "ms")