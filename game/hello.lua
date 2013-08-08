-- defines a factorial function
function fact (n)
    if n == 0 then
        return 1
    else
        return n * fact(n-1)
    end
end

-- prints hello world!
print("Hello world from Lua inside of prototype!")
print(fact(3))