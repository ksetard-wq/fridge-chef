#include <doctest/doctest.h>

#include "domain/Text.hpp"

TEST_CASE("normalize: регистр и пробелы") {
    CHECK(text::normalize("  Помидор  ") == "помидор");
    CHECK(text::normalize("МОЛОКО") == text::normalize("молоко"));
    CHECK(text::normalize("   ") == "");
}

TEST_CASE("normalize: единственное и множественное число") {
    CHECK(text::normalize("помидоры") == text::normalize("помидор"));
    CHECK(text::normalize("яйца") == text::normalize("яйцо"));
    CHECK(text::normalize("яблоки") == text::normalize("яблоко"));
}

TEST_CASE("normalize: ё и синонимы") {
    CHECK(text::normalize("свёкла") == text::normalize("свекла"));
    CHECK(text::normalize("картошка") == text::normalize("картофель"));
}

TEST_CASE("normalize: латиница") {
    CHECK(text::normalize("Cheese") == "cheese");
}
