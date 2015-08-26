/*!
@file
Defines `boost::hana::lexicographical_compare`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_LEXICOGRAPHICAL_COMPARE_HPP
#define BOOST_HANA_LEXICOGRAPHICAL_COMPARE_HPP

#include <boost/hana/fwd/lexicographical_compare.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/concept/iterable.hpp>
#include <boost/hana/core/dispatch.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/if.hpp>
#include <boost/hana/is_empty.hpp>
#include <boost/hana/less.hpp>
#include <boost/hana/tail.hpp>


namespace boost { namespace hana {
    //! @cond
    template <typename Xs, typename Ys>
    constexpr auto lexicographical_compare_t::operator()(Xs const& xs, Ys const& ys) const {
        return hana::lexicographical_compare(xs, ys, hana::less);
    }

    template <typename Xs, typename Ys, typename Pred>
    constexpr auto lexicographical_compare_t::operator()(Xs const& xs, Ys const& ys, Pred const& pred) const {
        using It1 = typename datatype<Xs>::type;
        using It2 = typename datatype<Ys>::type;
        using LexicographicalCompare = BOOST_HANA_DISPATCH_IF(
            lexicographical_compare_impl<It1>,
            _models<Iterable, It1>::value &&
            _models<Iterable, It2>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(_models<Iterable, It1>::value,
        "hana::lexicographical_compare(xs, ys, pred) requires 'xs' to be Iterable");

        static_assert(_models<Iterable, It2>::value,
        "hana::lexicographical_compare(xs, ys, pred) requires 'ys' to be Iterable");
    #endif

        return LexicographicalCompare::apply(xs, ys, pred);
    }
    //! @endcond

    template <typename It, bool condition>
    struct lexicographical_compare_impl<It, when<condition>> : default_ {
        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper2(Xs const&, Ys const&, Pred const&, decltype(hana::true_))
        { return hana::false_; }

        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper2(Xs const& xs, Ys const& ys, Pred const& pred, decltype(hana::false_))
        { return apply(hana::tail(xs), hana::tail(ys), pred); }

        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper2(Xs const& xs, Ys const& ys, Pred const& pred, bool is_greater)
        { return is_greater ? false : apply(hana::tail(xs), hana::tail(ys), pred); }


        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper1(Xs const&, Ys const&, Pred const&, decltype(hana::true_))
        { return hana::true_; }

        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper1(Xs const& xs, Ys const& ys, Pred const& pred, decltype(hana::false_))
        { return helper2(xs, ys, pred, hana::if_(pred(hana::front(ys), hana::front(xs)), hana::true_, hana::false_)); }

        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper1(Xs const& xs, Ys const& ys, Pred const& pred, bool is_less)
        { return is_less ? true : helper2(xs, ys, pred, hana::if_(pred(hana::front(ys), hana::front(xs)), hana::true_, hana::false_)); }


        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper(Xs const&, Ys const& ys, Pred const&, decltype(hana::true_))
        { return hana::not_(hana::is_empty(ys)); }

        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto
        helper(Xs const& xs, Ys const& ys, Pred const& pred, decltype(hana::false_))
        { return helper1(xs, ys, pred, hana::if_(pred(hana::front(xs), hana::front(ys)), hana::true_, hana::false_)); }


        template <typename Xs, typename Ys, typename Pred>
        static constexpr auto apply(Xs const& xs, Ys const& ys, Pred const& pred) {
            return helper(xs, ys, pred, hana::bool_<
                hana::value<decltype(hana::is_empty(xs))>() ||
                hana::value<decltype(hana::is_empty(ys))>()
            >);
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_LEXICOGRAPHICAL_COMPARE_HPP
