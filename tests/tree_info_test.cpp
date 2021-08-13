#include "catch2/catch_test_macros.hpp"

#include "regpossets.hpp"

TEST_CASE("Test one symbol pos sets", "[RegPosSets]") {
    RegPosSets pos_sets("a");

    CHECK(pos_sets.first().size() == 1);
    CHECK(pos_sets.last().size() == 1);
    CHECK(pos_sets.first() == pos_sets.last());
    CHECK(!pos_sets.is_nullable());

    auto pos = *pos_sets.first().cbegin();

    CHECK(pos_sets.follow(pos).empty());
}

TEST_CASE("Test Concat", "[RegPosSets]") {
    RegPosSets a_pos_sets("a"), b_pos_sets("b");
    auto ab_pos_sets = a_pos_sets.concat(b_pos_sets);

    CHECK(ab_pos_sets.first().size() == 1);
    CHECK(ab_pos_sets.last().size() == 1);
    CHECK(ab_pos_sets.first() == a_pos_sets.first());
    CHECK(ab_pos_sets.last() == b_pos_sets.last());
    CHECK(!ab_pos_sets.is_nullable());

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();

    CHECK(ab_pos_sets.follow(pos_a) == b_pos_sets.first());
    CHECK(ab_pos_sets.follow(pos_b).empty());
}

TEST_CASE("Test Or", "[RegPosSets]") {
    RegPosSets a_pos_sets("a"), b_pos_sets("b");
    auto a_or_b_pos_sets = a_pos_sets.or(b_pos_sets);

    auto ab_first_union = a_pos_sets.first();
    ab_first_union.insert(
        b_pos_sets.first().cbegin(),
        b_pos_sets.first().cend()
    );

    auto ab_last_union = a_pos_sets.last();
    ab_last_union.insert(
        b_pos_sets.last().cbegin(),
        b_pos_sets.last().cend()
    );

    CHECK(a_or_b_pos_sets.first().size() == 2);
    CHECK(a_or_b_pos_sets.last().size() == 2);
    CHECK(a_or_b_pos_sets.first() == ab_first_union);
    CHECK(a_or_b_pos_sets.last() == ab_last_union);
    CHECK(!a_or_b_pos_sets.is_nullable());

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();

    CHECK(a_or_b_pos_sets.follow(pos_a).empty());
    CHECK(a_or_b_pos_sets.follow(pos_b).empty());
}

TEST_CASE("Test Star", "[RegPosSets]") {
    RegPosSets pos_sets("a");
    auto star_pos_sets = pos_sets.star();

    CHECK(star_pos_sets.first().size() == 1);
    CHECK(star_pos_sets.last().size() == 1);
    CHECK(star_pos_sets.first() == star_pos_sets.last());
    CHECK(star_pos_sets.is_nullable());

    auto pos = *star_pos_sets.first().cbegin();

    CHECK(star_pos_sets.follow(pos) == star_pos_sets.last());
}

TEST_CASE("Test Concat Or", "[RegPosSets]") {
    RegPosSets
        a_pos_sets("a"),
        b_pos_sets("b"),
        c_pos_sets("c");

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();
    auto pos_c = *c_pos_sets.first().cbegin();

    auto ab_pos_sets = a_pos_sets.concat(b_pos_sets);
    auto ab_or_c_pos_set = ab_pos_sets.or(c_pos_sets);

    auto expected_first = ab_pos_sets.first();
    expected_first.insert(
        c_pos_sets.first().cbegin(),
        c_pos_sets.first().cend()
    );

    auto expected_last = ab_pos_sets.last();
    expected_last.insert(
        c_pos_sets.last().cbegin(),
        c_pos_sets.last().cend()
    );

    CHECK(ab_or_c_pos_set.first() == expected_first);
    CHECK(ab_or_c_pos_set.last() == expected_last);
    CHECK(ab_or_c_pos_set.follow(pos_a) == b_pos_sets.first());
    CHECK(ab_or_c_pos_set.follow(pos_b).empty());
    CHECK(ab_or_c_pos_set.follow(pos_c).empty());
    CHECK(!ab_or_c_pos_set.is_nullable());
}

TEST_CASE("Test Or Concat", "[RegPosSets]") {
    RegPosSets
        a_pos_sets("a"),
        b_pos_sets("b"),
        c_pos_sets("c");

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();
    auto pos_c = *c_pos_sets.first().cbegin();

    auto a_or_b_pos_sets = a_pos_sets.or(b_pos_sets);
    auto a_or_b_c_pos_set = a_or_b_pos_sets.concat(c_pos_sets);

    auto expected_first = a_or_b_pos_sets.first();
    auto expected_last = c_pos_sets.last();

    CHECK(a_or_b_c_pos_set.first() == expected_first);
    CHECK(a_or_b_c_pos_set.last() == expected_last);
    CHECK(a_or_b_c_pos_set.follow(pos_a) == c_pos_sets.first());
    CHECK(a_or_b_c_pos_set.follow(pos_b) == c_pos_sets.first());
    CHECK(a_or_b_c_pos_set.follow(pos_c).empty());
    CHECK(!a_or_b_c_pos_set.is_nullable());
}

TEST_CASE("Test Concat Star", "[RegPosSets]") {
    RegPosSets
        a_pos_sets("a"),
        b_pos_sets("b");

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();

    auto b_star_pos_set = b_pos_sets.star();
    auto expr_pos_sets = a_pos_sets.concat(b_star_pos_set);

    auto expected_first = a_pos_sets.first();
    auto expected_last = a_pos_sets.last();
    expected_last.insert(
        b_star_pos_set.last().cbegin(),
        b_star_pos_set.last().cend()
    );

    CHECK(expr_pos_sets.first() == expected_first);
    CHECK(expr_pos_sets.last() == expected_last);
    CHECK(expr_pos_sets.follow(pos_a) == b_star_pos_set.first());
    CHECK(expr_pos_sets.follow(pos_b) == b_star_pos_set.first());
    CHECK(!expr_pos_sets.is_nullable());
}

TEST_CASE("Test Star Concat", "[RegPosSets]") {
    RegPosSets
        a_pos_sets("a"),
        b_pos_sets("b");

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();

    auto a_star_pos_set = a_pos_sets.star();
    auto expr_pos_sets = a_star_pos_set.concat(b_pos_sets);

    auto expected_first = a_star_pos_set.first();
    expected_first.insert(
        b_pos_sets.first().cbegin(),
        b_pos_sets.first().cend()
    );

    auto expected_last = b_pos_sets.last();

    auto expected_follow_a = a_star_pos_set.first();
    expected_follow_a.insert(
        b_pos_sets.first().cbegin(),
        b_pos_sets.first().cend()
    );

    CHECK(expr_pos_sets.first() == expected_first);
    CHECK(expr_pos_sets.last() == expected_last);
    CHECK(expr_pos_sets.follow(pos_a) == expected_follow_a);
    CHECK(expr_pos_sets.follow(pos_b).empty());
    CHECK(!expr_pos_sets.is_nullable());
}

TEST_CASE("Test Or Star", "[RegPosSets]") {
    RegPosSets
        a_pos_sets("a"),
        b_pos_sets("b");

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();

    auto b_star_pos_set = b_pos_sets.star();
    auto expr_pos_sets = a_pos_sets.or(b_star_pos_set);

    auto expected_first = a_pos_sets.first();
    expected_first.insert(
        b_star_pos_set.first().cbegin(),
        b_star_pos_set.first().cend()
    );

    auto expected_last = a_pos_sets.last();
    expected_last.insert(
        b_star_pos_set.last().cbegin(),
        b_star_pos_set.last().cend()
    );

    CHECK(expr_pos_sets.first() == expected_first);
    CHECK(expr_pos_sets.last() == expected_last);
    CHECK(expr_pos_sets.follow(pos_a).empty());
    CHECK(expr_pos_sets.follow(pos_b) == b_star_pos_set.first());
    CHECK(expr_pos_sets.is_nullable());
}

TEST_CASE("Test Star Or", "[RegPosSets]") {
    RegPosSets
        a_pos_sets("a"),
        b_pos_sets("b");

    auto pos_a = *a_pos_sets.first().cbegin();
    auto pos_b = *b_pos_sets.first().cbegin();

    auto a_star_pos_set = a_pos_sets.star();
    auto expr_pos_sets = a_star_pos_set.or(b_pos_sets);

    auto expected_first = a_star_pos_set.first();
    expected_first.insert(
        b_pos_sets.first().cbegin(),
        b_pos_sets.first().cend()
    );

    auto expected_last = a_star_pos_set.last();
    expected_last.insert(
        b_pos_sets.last().cbegin(),
        b_pos_sets.last().cend()
    );

    CHECK(expr_pos_sets.first() == expected_first);
    CHECK(expr_pos_sets.last() == expected_last);
    CHECK(expr_pos_sets.follow(pos_a) == a_star_pos_set.first());
    CHECK(expr_pos_sets.follow(pos_b).empty());
    CHECK(expr_pos_sets.is_nullable());
}

TEST_CASE("Test generic expr", "[RegPosSets]") {
    RegPosSets a[] = {
        "a",
        "a"
    };

    RegPosSets b[] = {
        "b",
        "b",
        "b"
    };

    auto pos_a = [&](auto idx) {
        return *a[idx].first().cbegin();
    };

    auto pos_b = [&](auto idx) {
        return *b[idx].first().cbegin();
    };

    auto a_or_b_star = a[0].or(b[0]).star();
    auto expr = a_or_b_star
            .concat(a[1])
            .concat(b[1])
            .concat(b[2]);

    auto expected_first = a_or_b_star.first();
    expected_first.insert(
        a[1].first().cbegin(),
        a[1].first().cend()
    );

    auto expected_last = b[2].last();

    CHECK(expr.first() == expected_first);
    CHECK(expr.last() == expected_last);
    CHECK(expr.follow(pos_a(0)) == expected_first);
    CHECK(expr.follow(pos_b(0)) == expected_first);
    CHECK(expr.follow(pos_a(1)) == b[1].first());
    CHECK(expr.follow(pos_b(1)) == b[2].first());
    CHECK(expr.follow(pos_b(2)).empty());
    CHECK(!expr.is_nullable());
}
