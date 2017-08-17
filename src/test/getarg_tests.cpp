#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-brc");
    BOOST_CHECK(GetBoolArg("-brc"));
    BOOST_CHECK(GetBoolArg("-brc", false));
    BOOST_CHECK(GetBoolArg("-brc", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-fooo"));
    BOOST_CHECK(!GetBoolArg("-fooo", false));
    BOOST_CHECK(GetBoolArg("-fooo", true));

    ResetArgs("-brc=0");
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", false));
    BOOST_CHECK(!GetBoolArg("-brc", true));

    ResetArgs("-brc=1");
    BOOST_CHECK(GetBoolArg("-brc"));
    BOOST_CHECK(GetBoolArg("-brc", false));
    BOOST_CHECK(GetBoolArg("-brc", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nofoo");
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", false));
    BOOST_CHECK(!GetBoolArg("-brc", true));

    ResetArgs("-nofoo=1");
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", false));
    BOOST_CHECK(!GetBoolArg("-brc", true));

    ResetArgs("-brc -nofoo");  // -brc should win
    BOOST_CHECK(GetBoolArg("-brc"));
    BOOST_CHECK(GetBoolArg("-brc", false));
    BOOST_CHECK(GetBoolArg("-brc", true));

    ResetArgs("-brc=1 -nofoo=1");  // -brc should win
    BOOST_CHECK(GetBoolArg("-brc"));
    BOOST_CHECK(GetBoolArg("-brc", false));
    BOOST_CHECK(GetBoolArg("-brc", true));

    ResetArgs("-brc=0 -nofoo=0");  // -brc should win
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", false));
    BOOST_CHECK(!GetBoolArg("-brc", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--brc=1");
    BOOST_CHECK(GetBoolArg("-brc"));
    BOOST_CHECK(GetBoolArg("-brc", false));
    BOOST_CHECK(GetBoolArg("-brc", true));

    ResetArgs("--nofoo=1");
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", false));
    BOOST_CHECK(!GetBoolArg("-brc", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-brc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-brc", "eleven"), "eleven");

    ResetArgs("-brc -bar");
    BOOST_CHECK_EQUAL(GetArg("-brc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-brc", "eleven"), "");

    ResetArgs("-brc=");
    BOOST_CHECK_EQUAL(GetArg("-brc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-brc", "eleven"), "");

    ResetArgs("-brc=11");
    BOOST_CHECK_EQUAL(GetArg("-brc", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-brc", "eleven"), "11");

    ResetArgs("-brc=eleven");
    BOOST_CHECK_EQUAL(GetArg("-brc", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-brc", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-brc", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-brc", 0), 0);

    ResetArgs("-brc -bar");
    BOOST_CHECK_EQUAL(GetArg("-brc", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-brc=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-brc", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-brc=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-brc", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--brc");
    BOOST_CHECK_EQUAL(GetBoolArg("-brc"), true);

    ResetArgs("--brc=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-brc", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nofoo");
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", true));
    BOOST_CHECK(!GetBoolArg("-brc", false));

    ResetArgs("-nofoo=1");
    BOOST_CHECK(!GetBoolArg("-brc"));
    BOOST_CHECK(!GetBoolArg("-brc", true));
    BOOST_CHECK(!GetBoolArg("-brc", false));

    ResetArgs("-nofoo=0");
    BOOST_CHECK(GetBoolArg("-brc"));
    BOOST_CHECK(GetBoolArg("-brc", true));
    BOOST_CHECK(GetBoolArg("-brc", false));

    ResetArgs("-brc --nofoo");
    BOOST_CHECK(GetBoolArg("-brc"));

    ResetArgs("-nofoo -brc"); // brc always wins:
    BOOST_CHECK(GetBoolArg("-brc"));
}

BOOST_AUTO_TEST_SUITE_END()
