#ifndef BOOST_SERIALIZATION_BOOST_VECTOR_HPP
#define BOOST_SERIALIZATION_BOOST_VECTOR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
#pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// boost_vector.hpp: serialization for boost vector templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// fast array serialization (C) Copyright 2005 Matthias Troyer
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/container/vector.hpp>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/archive/detail/basic_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/mpl/bool_fwd.hpp>
#include <boost/mpl/if.hpp>
#include <boost/serialization/array_wrapper.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/split_free.hpp>

// default is being compatible with version 1.34.1 files, not 1.35 files
#ifndef BOOST_SERIALIZATION_VECTOR_VERSIONED
#define BOOST_SERIALIZATION_VECTOR_VERSIONED(V) (V == 4 || V == 5)
#endif

namespace boost {
namespace serialization {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector< T >

// the default versions

template <class Archive, class U, class Allocator, class Options>
inline void save(Archive& ar, const boost::container::vector<U, Allocator, Options>& t,
                 const unsigned int /* file_version */, mpl::false_) {
    boost::serialization::stl::save_collection<Archive,
                                               boost::container::vector<U, Allocator, Options>>(ar,
                                                                                                t);
}

template <class Archive, class U, class Allocator, class Options>
inline void load(Archive& ar, boost::container::vector<U, Allocator, Options>& t,
                 const unsigned int /* file_version */, mpl::false_) {
    const boost::archive::library_version_type library_version(ar.get_library_version());
    // retrieve number of elements
    item_version_type item_version(0);
    collection_size_type count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    if (boost::archive::library_version_type(3) < library_version) {
        ar >> BOOST_SERIALIZATION_NVP(item_version);
    }
    t.reserve(count);
    stl::collection_load_impl(ar, t, count, item_version);
}

// the optimized versions

template <class Archive, class U, class Allocator, class Options>
inline void save(Archive& ar, const boost::container::vector<U, Allocator, Options>& t,
                 const unsigned int /* file_version */, mpl::true_) {
    const collection_size_type count(t.size());
    ar << BOOST_SERIALIZATION_NVP(count);
    if (!t.empty())
        // explict template arguments to pass intel C++ compiler
        ar << serialization::make_array<const U, collection_size_type>(static_cast<const U*>(&t[0]),
                                                                       count);
}

template <class Archive, class U, class Allocator, class Options>
inline void load(Archive& ar, boost::container::vector<U, Allocator, Options>& t,
                 const unsigned int /* file_version */, mpl::true_) {
    collection_size_type count(t.size());
    ar >> BOOST_SERIALIZATION_NVP(count);
    t.resize(count);
    unsigned int item_version = 0;
    if (BOOST_SERIALIZATION_VECTOR_VERSIONED(ar.get_library_version())) {
        ar >> BOOST_SERIALIZATION_NVP(item_version);
    }
    if (!t.empty())
        // explict template arguments to pass intel C++ compiler
        ar >> serialization::make_array<U, collection_size_type>(static_cast<U*>(&t[0]), count);
}

// dispatch to either default or optimized versions

template <class Archive, class U, class Allocator, class Options>
inline void save(Archive& ar, const boost::container::vector<U, Allocator, Options>& t,
                 const unsigned int file_version) {
    typedef typename boost::serialization::use_array_optimization<Archive>::template apply<
        typename remove_const<U>::type>::type use_optimized;
    save(ar, t, file_version, use_optimized());
}

template <class Archive, class U, class Allocator, class Options>
inline void load(Archive& ar, boost::container::vector<U, Allocator, Options>& t,
                 const unsigned int file_version) {
#ifdef BOOST_SERIALIZATION_VECTOR_135_HPP
    if (ar.get_library_version() == boost::archive::library_version_type(5)) {
        load(ar, t, file_version, boost::is_arithmetic<U>());
        return;
    }
#endif
    typedef typename boost::serialization::use_array_optimization<Archive>::template apply<
        typename remove_const<U>::type>::type use_optimized;
    load(ar, t, file_version, use_optimized());
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template <class Archive, class U, class Allocator, class Options>
inline void serialize(Archive& ar, boost::container::vector<U, Allocator, Options>& t,
                      const unsigned int file_version) {
    boost::serialization::split_free(ar, t, file_version);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template <class Archive, class Allocator, class Options>
inline void serialize(Archive& ar, boost::container::vector<bool, Allocator, Options>& t,
                      const unsigned int file_version) {
    boost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace boost

#include <boost/serialization/collection_traits.hpp>

BOOST_SERIALIZATION_COLLECTION_TRAITS(boost::container::vector)

#endif // BOOST_SERIALIZATION_BOOST_VECTOR_HPP
