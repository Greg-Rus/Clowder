test_variable = 100

config   = {
    fullscreen = false,
    resolution = {
        width = 800,
        height = 600
    }
}

function factorial(n)
    local result = 1
    for i=1, n do
        result = result*i
    end
    return result
end

print ("Cube of 3: "..cube(3))