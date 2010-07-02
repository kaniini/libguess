#include "libguess.h"

mowgli_patricia_t *guess_impl_list = NULL;

void strcasecanon(char *str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

static void
guess_impl_register(const char *lang, guess_impl_f impl)
{
    return_if_fail(guess_impl_list != NULL);

    mowgli_patricia_add(guess_impl_list, lang, impl);
}

static void
guess_init(void)
{
    mowgli_init();

    /* check if already initialized */
    if (guess_impl_list != NULL)
        return;

    guess_impl_list = mowgli_patricia_create(strcasecanon);

    guess_impl_register(GUESS_REGION_JP, guess_jp);
    guess_impl_register(GUESS_REGION_TW, guess_tw);
    guess_impl_register(GUESS_REGION_CN, guess_cn);
    guess_impl_register(GUESS_REGION_KR, guess_kr);
    guess_impl_register(GUESS_REGION_RU, guess_ru);
    guess_impl_register(GUESS_REGION_AR, guess_ar);
    guess_impl_register(GUESS_REGION_TR, guess_tr);
    guess_impl_register(GUESS_REGION_GR, guess_gr);
    guess_impl_register(GUESS_REGION_HW, guess_hw);
    guess_impl_register(GUESS_REGION_PL, guess_pl);
}

const char *
libguess_determine_encoding(const char *inbuf, int buflen, const char *lang)
{
    guess_impl_f impl;

    guess_init();

    impl = mowgli_patricia_retrieve(guess_impl_list, lang);
    if (impl != NULL)
        return impl(inbuf, buflen);

    /* TODO: try other languages as fallback? */
    return NULL;
}
