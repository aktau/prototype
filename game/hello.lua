-- defines a factorial function
function fact (n)
    if n == 0 then
        return 1
    else
        return n * fact(n-1)
    end
end

local fn = 5

-- prints hello world!
print("Hello world from Lua inside of prototype!")
print("The facturial of " .. fn .. " is " .. fact(fn))