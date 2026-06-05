add_library(xprintf STATIC
    src/xprintf.c
)

target_include_directories(xprintf PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

# 必要なら定義を追加
# target_compile_definitions(xprintf PRIVATE XF_USE_LLI)