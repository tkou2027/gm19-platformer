#include "sound_config.h"

namespace
{
    std::wstring g_bgm_paths[static_cast<size_t>(foo::BgmName::MAX)]
    {
        L"sound/bgm_loop_lightless_dawn.wav",
        L"sound/bgm_loop_equatorial_complex.wav",
        L"sound/bgm_loop_mixkit-light-rain-loop-2393.wav",
    };

    std::wstring g_se_paths[static_cast<size_t>(foo::SeName::MAX)]
    {
        L"sound/se_click_maou_se_system41.wav",
        L"sound/se_item_get_maou_se_system27.wav",
        L"sound/se_hit_maou_se_system41.wav",
        L"sound/se_die_maou_se_system05.wav",
    };
}

namespace foo
{
    std::wstring SoundManager::GetBgmPath(BgmName name)
    {
        return g_bgm_paths[static_cast<size_t>(name)];
    }

    std::wstring SoundManager::GetSePath(SeName name)
    {
        return g_se_paths[static_cast<size_t>(name)];
    }
}
