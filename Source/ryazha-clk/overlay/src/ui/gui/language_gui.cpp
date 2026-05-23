/*
 * Copyright (c) Ryazha-CLK Contributors
 *
 * See language_gui.h.
 *
 */

#include "language_gui.h"

#include "../../i18n.hpp"

#include <tesla.hpp>

// Используем HOS-extra-font символ "" -- такой же checkmark, которым
// рисует Tesla в системных меню. Plain "✓" (U+2713) НЕ входит в bundled
// stb_truetype font'е оверлея, поэтому раньше юзер видел пустое место
// или прямоугольник вместо галочки.
//
// Через ult::CHECKMARK_SYMBOL не идём, чтобы не тащить include всего
// global_vars.hpp в gui-слой -- символ короткий, локального constexpr
// достаточно.
namespace {
constexpr const char* CHECK_MARK = "";
} // namespace

tsl::elm::Element* LanguageGui::baseUI() {
    // Минимальный baseUI без 35-px spacer'а. listElement обязательно
    // должен быть выставлен -- иначе listUI() упадёт на nullptr->addItem.
    auto* list = new tsl::elm::List();
    this->listElement = list;
    this->listUI();
    return list;
}

void LanguageGui::listUI() {
    // ВАЖНО: baseUI() добавил 35-px spacer перед нашим contentом, плюс
    // BaseFrame резервирует header 234px. Для language-меню (короткий список)
    // это даёт огромный пустой блок сверху. CategoryHeader тоже добавляет
    // ~40px собственного padding'а -- двойной заголовок выглядит ужасно.
    // Решение: НЕ добавляем CategoryHeader, метку рисуем прямо в самом
    // первом item'е (если так хочется) или вовсе без неё. У нас слева
    // итак стоит "Ryazha-clk" badge -- юзеру и так понятно, где он.

    const std::string current = i18n::CurrentLanguage();

    for (const auto& lang : i18n::AvailableLanguages()) {
        const std::string code   = lang.code;
        const std::string native = lang.native;

        auto* item = new tsl::elm::ListItem(native);
        if (code == current) item->setValue(CHECK_MARK);

        item->setClickListener([code](u64 keys) {
            if (!(keys & HidNpadButton_A)) return false;
            // Применяем язык (пишет в config.ini + перезагружает таблицу).
            i18n::ApplyLanguage(code);

            // ВАЖНО: swapTo, НЕ changeTo.
            //
            // changeTo<LanguageGui>() PUSHES новую LanguageGui на nav-stack:
            //   [Main, LangGui_ru, LangGui_en, LangGui_uk, ...]
            // Юзер тыкает 3 языка -> в стэке 4 LangGui, и B-кнопка
            // последовательно "откатывает" по одному. Выглядит как
            // "back rolls back action instead of leaving menu".
            //
            // swapTo<LanguageGui>(1) ЗАМЕНЯЕТ текущую LangGui на свежую,
            // не растя стэк. После любого числа кликов в Language menu
            // одно нажатие B возвращает прямо в Main.
            tsl::swapTo<LanguageGui>();
            return true;
        });

        this->listElement->addItem(item);
    }
}

void LanguageGui::refresh() {
    // Список статичен на протяжении жизни меню -- ApplyLanguage делает
    // полный rebuild через changeTo<LanguageGui>(), refresh не нужен.
}
