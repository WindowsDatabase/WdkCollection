@echo off
setlocal

for %%f in (*.c) do (
  espsmvprint %* %%f
)

for %%f in (*.cpp) do (
  espsmvprint %* %%f
)

for %%f in (*.cxx) do (
  espsmvprint %* %%f
)

endlocal
