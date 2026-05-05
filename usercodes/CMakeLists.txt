# usercodes内のソースファイルを変数にまとめる
file(GLOB USER_SOURCES
    "sac/*.c"
    "sac/*/*.c"
    "common/*.c"
    "app/*.c"
    "app/*/*.c"
)

# usercodes内のヘッダーファイル検索パスを追加
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/sac
)

# ソースをルートプロジェクトのターゲットに追加
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SOURCES}
)