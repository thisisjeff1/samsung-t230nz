dtb-y += exynos7870-degasy18wifi_eur_bnz_00.dtb
dtb-y += exynos7870-degasy18wifi_eur_bnz_01.dtb
dtb-y += exynos7870-degasy18wifi_eur_bnz_02.dtb

targets += dtbs
DTB_LIST  := $(dtb-y) $(dtbo-y)

dtbs: $(addprefix $(obj)/, $(DTB_LIST))

clean-files := *.dtb*
