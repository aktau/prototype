-- load the ffi library
local ffi = require("ffi")

ffi.cdef[[
void Sleep(int ms);
int poll(struct pollfd *fds, unsigned long nfds, int timeout);
]]

local sleep
if ffi.os == "Windows" then
  function sleep(s)
    ffi.C.Sleep(s*1000)
  end
else
  function sleep(s)
    ffi.C.poll(nil, 0, s*1000)
  end
end

-- defines a factorial function
function fact (n)
    if n == 0 then
        return 1
    else
        return n * fact(n-1)
    end
end

local fn = 5
local moar = 10

print("lua interpreter compatibility version: " .. tostring(_VERSION))
print("jit status: " .. tostring(jit.status()))
io.write("luajit ffi: ")
io.flush()
sleep(0.01)
print("works");

print("The factorial of " .. fn .. " is " .. fact(fn))
print("The factorial of " .. moar .. " is " .. fact(moar))