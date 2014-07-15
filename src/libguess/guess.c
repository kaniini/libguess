#include <strings.h>

#include "libguess.h"

struct guess_impl {
    const char *lang;
    guess_impl_f impl;
};

static const struct guess_impl guess_impl_list[] = {
    {GUESS_REGION_JP, guess_jp},
    {GUESS_REGION_TW, guess_tw},
    {GUESS_REGION_CN, guess_cn},
    {GUESS_REGION_KR, guess_kr},
    {GUESS_REGION_RU, guess_ru},
    {GUESS_REGION_AR, guess_ar},
    {GUESS_REGION_TR, guess_tr},
    {GUESS_REGION_GR, guess_gr},
    {GUESS_REGION_HW, guess_hw},
    {GUESS_REGION_PL, guess_pl},
    {GUESS_REGION_BL, guess_bl},
};

static guess_impl_f
guess_find_impl(const char *lang)
{
    int i;

    for (i = 0; i < sizeof(guess_impl_list) / sizeof(guess_impl_list[0]); i++) {
        if (!strcasecmp(guess_impl_list[i].lang, lang))
            return guess_impl_list[i].impl;
    }

    return 0;
}

void
libguess_init(void)
{
    /* provided for API compatibility with older versions */
}

const char *
libguess_determine_encoding(const char *inbuf, int buflen, const char *lang)
{
    guess_impl_f impl = guess_find_impl(lang);

    if (impl != NULL)
        return impl(inbuf, buflen);

    /* TODO: try other languages as fallback? */
    return NULL;
}
