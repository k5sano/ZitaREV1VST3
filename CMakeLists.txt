cmake_minimum_required(VERSION 3.24)
project(ZitaRev1 VERSION 0.1.0)

# --------------------------------------------------------------------------
# 文字コード / MSVC 設定
# --------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/wd4819 /utf-8)
endif()

# --------------------------------------------------------------------------
# JUCE を追加
# --------------------------------------------------------------------------
# JUCE_PATH 環境変数 → 未設定なら ../JUCE を参照
if(DEFINED ENV{JUCE_PATH})
    set(JUCE_DIR "$ENV{JUCE_PATH}")
else()
    set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE")
endif()
add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")

# --------------------------------------------------------------------------
# プラグイン定義
# --------------------------------------------------------------------------
juce_add_plugin(ZitaRev1
    COMPANY_NAME           "OpenSource"
    IS_SYNTH               FALSE
    NEEDS_MIDI_INPUT       FALSE
    NEEDS_MIDI_OUTPUT      FALSE
    PLUGIN_MANUFACTURER_CODE Opsc
    PLUGIN_CODE            Ztr1
    FORMATS                VST3 Standalone
    PRODUCT_NAME           "ZitaRev1"
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# --------------------------------------------------------------------------
# JuceHeader.h を生成
# --------------------------------------------------------------------------
juce_generate_juce_header(ZitaRev1)

# --------------------------------------------------------------------------
# ソース
# --------------------------------------------------------------------------
target_sources(ZitaRev1
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/zita/reverb.cc
        Source/zita/pareq.cc
)

# --------------------------------------------------------------------------
# コンパイル定義
# --------------------------------------------------------------------------
target_compile_definitions(ZitaRev1
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
    PRIVATE
        _USE_MATH_DEFINES   # MSVC で M_PI を有効化
)

# --------------------------------------------------------------------------
# インクルードパス
# --------------------------------------------------------------------------
target_include_directories(ZitaRev1
    PRIVATE
        Source
        Source/zita
)

# --------------------------------------------------------------------------
# リンクライブラリ
# --------------------------------------------------------------------------
target_link_libraries(ZitaRev1
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
