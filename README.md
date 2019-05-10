# Documents processing - Image Binarization

Реализован алгоритм бинаризации Niblack

Based on

[1] https://www.researchgate.net/publication/307612238_Fast_implementation_of_the_Niblack_binarization_algorithm_for_microscope_image_segmentation

[2] https://www.researchgate.net/publication/221253803_Comparison_of_Niblack_inspired_Binarization_Methods_for_Ancient_Documents

В приложенном датасете суммарно `111,8` Мпикс, они обработались за `280.6` секунд. (Intel® Core™ i7-8750H CPU @ 2.20GHz × 12)

Скорость приблизительно `0,4` Мпикс/сек

Параметры по умолчанию: стартовый размер окна 30 пикс., коэффициент `k` = -1.

# Requirements

CImg library

# How to run

В папке выполните:

```
make
./runall.sh
```

На готовый результат работы можно посмотреть в архиве `processed.zip`.

# Плюсы и минусы алгоритма, какие проблемы

* Плохо работает на изображениях, где белый текст изображен на черном фоне
* Хорошо выделяет фрагменты черного текста на белом фоне, но много шумов на фоне без текста. Эта проблема еще описывалась  в статье [1]. Можно попробовать прикрутить denoiser, или исопльзовать чуть более продвинутые техники.
* В алгоритме практически нет гиперпараметров, основной гиперпараметр -- `k` (вес стандартного отклонения в формуле для threshold).

