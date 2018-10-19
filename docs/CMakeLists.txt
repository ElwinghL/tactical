find_package(LATEX)

set(LATEX_COMPILER_FLAGS "-interaction=nonstopmode -shell-escape" CACHE STRING "Flags passed to latex.")
set(PDFLATEX_COMPILER_FLAGS "-interaction=nonstopmode -shell-escape" CACHE STRING "Flags passed to pdflatex.")

include(UseLATEX.cmake)

add_subdirectory(GDD)
add_subdirectory(UML)
add_subdirectory(coding_standard)
