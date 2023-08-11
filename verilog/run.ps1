iverilog.exe -o .\build\cpu .\src\memory.v .\test\memory_tb.v

vvp.exe .\build\cpu

gtkwave.exe .\build\memory_tb.vcd