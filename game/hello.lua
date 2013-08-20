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
print("The facturial of " .. fn .. " is " .. fact(fn))
print("The facturial of " .. moar .. " is " .. fact(moar))