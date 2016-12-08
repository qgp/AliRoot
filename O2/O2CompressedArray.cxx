/// \file CompressedArray.cxx
/// \brief implementation of the CompressedArray class.
/// \since 2016-12-05
/// \author R.G.A. Deckers
/// \copyright
///  This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 3 of
/// the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details at
/// https://www.gnu.org/copyleft/gpl.html

#include "O2CompressedArray.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <utilities/logging.h>
#include <zfp/zfp.h>

using O2::CompressedArray;

typedef struct {
  std::multimap<uint32_t, uint8_t, std::greater<uint32_t>> bytes;
  std::multimap<uint32_t, uint16_t, std::greater<uint32_t>> words;
  std::multimap<uint32_t, uint32_t, std::greater<uint32_t>> doublewords;
  std::multimap<uint32_t, uint64_t, std::greater<uint32_t>> quadwords;
} multimap_t;

template <typename T>
size_t estimate_compression(
    const std::multimap<uint32_t, T, std::greater<uint32_t>> &mapping) {
  size_t element_size = sizeof(T);
  // report(INFO, "element size = %lu", element_size);
  size_t element_count = 0;
  for (const auto &p : mapping) {
    element_count += p.first;
  }
  // report(INFO, "total hits = %lu", byte_size);
  size_t byte_size = element_count * element_size;
  size_t best_size = byte_size;
  // report(INFO, "bytes:\t%8lu", byte_size);
  // Check if field+index is best
  // field would take 1 element (base) + size (2 bytes) + (index (2
  // bytes)+element)*elements
  // others.
  size_t field_size =
      element_size + 2 +
      (2 + element_size) * (byte_size - element_size * mapping.begin()->first);
  // report(INFO, "field:\t%8lu", field_size);
  best_size = field_size < best_size ? field_size : best_size;
  std::vector<uint32_t> huffman_buffer;
  { // initialize the buffer with the top counts.
    for (auto it = mapping.begin(); it != mapping.end(); it++) {
      huffman_buffer.push_back(it->first);
    }
  }
  // report(INFO, "buffer size = %lu",
  //        huffman_buffer.size()); // amount of unique codes.
  for (size_t huffman_bits = 1;
       ((1 << huffman_bits) - 1) < huffman_buffer.size(); huffman_bits++) {
    size_t total_indexed = 0;
    size_t bits_per_index = huffman_bits;
    for (size_t i = 0; i < (1 << huffman_bits) - 1; i++) {
      total_indexed += huffman_buffer[i];
    }
    size_t overhead = ((1 << huffman_bits) - 1) * element_size;
    size_t indexed_size_bits = (total_indexed * bits_per_index);
    //(total elements-indexed elements)= remaining elements. multiplied by
    // elements size if in bytes*8+bits per index to determine total bits
    size_t remaining_size_bits =
        (element_count - total_indexed) * (element_size * 8 + bits_per_index);
    size_t total_size_bytes =
        overhead + (indexed_size_bits + remaining_size_bits + 7) / 8;
    best_size = total_size_bytes < best_size ? total_size_bytes : best_size;
    // report(INFO, "Huffman%lu:\t%8lu [%lu/%lu]", bits_per_index,
    //        total_size_bytes, indexed_size_bits, (remaining_size_bits));
  }
  for (size_t huffman_bits = 2;
       (huffman_bits < huffman_buffer.size()) && huffman_bits < 128;
       huffman_bits++) {
    size_t total_indexed = 0;
    size_t total_indexed_bits = 0;
    for (size_t i = 0; i < huffman_bits; i++) {
      total_indexed += huffman_buffer[i];
      total_indexed_bits += huffman_buffer[i] * (i + 1);
    }
    //(total elements-indexed elements)= remaining elements. multiplied by
    // elements size if in bytes*8+bits per index to determine total bits
    size_t overhead = huffman_bits * element_size;
    size_t remaining_size_bits =
        (element_count - total_indexed) * (element_size * 8 + huffman_bits);
    size_t total_size_bytes =
        overhead + (total_indexed_bits + remaining_size_bits + 7) / 8;
    best_size = total_size_bytes < best_size ? total_size_bytes : best_size;
    // report(INFO, "DHuffman%lu:\t%8lu [%lu/%lu]", huffman_bits,
    // total_size_bytes,
    //        total_indexed_bits, (remaining_size_bits));
  }
  // report(WARN, "%lu", best_size);
  // Entropy encoding strategies:
  // * fixed huffman coding, encode as [top_x] + others. e.g. top 7 = 3 bits
  // header. top 7 take 3 bits, rest takes 3 bits more.
  // * variable, top one is 0, other is 1. 01 means second one. 011 means third,
  // 0111 means fourth, x1111 means other.
  // *
  return best_size;
  // report(INFO, "field size = %lu", field_size);
}

multimap_t frequency_map(const std::vector<uint8_t> &vec) {
  const unsigned char *data = vec.data();
  size_t size = vec.size();
  multimap_t mapping;
  std::map<uint8_t, uint32_t> mapbytes;
  std::map<uint16_t, uint32_t> mapwords;
  std::map<uint32_t, uint32_t> mapdoublewords;
  std::map<uint64_t, uint32_t> mapquadwords;
  uint8_t *bytes = (uint8_t *)data;
  uint16_t *words = (uint16_t *)data;
  uint32_t *doublewords = (uint32_t *)data;
  uint64_t *quadwords = (uint64_t *)data;
  // TODO: padding
  for (size_t u = 0; u < size / 8; u++) {
    // one qw
    mapquadwords[quadwords[u]]++;

    // one dw
    mapdoublewords[doublewords[2 * u + 0]]++;
    mapdoublewords[doublewords[2 * u + 1]]++;

    // 4 words
    mapwords[words[4 * u + 0]]++;
    mapwords[words[4 * u + 1]]++;
    mapwords[words[4 * u + 2]]++;
    mapwords[words[4 * u + 3]]++;

    // add 8 bytes
    mapbytes[bytes[8 * u + 0]]++;
    mapbytes[bytes[8 * u + 1]]++;
    mapbytes[bytes[8 * u + 2]]++;
    mapbytes[bytes[8 * u + 3]]++;
    mapbytes[bytes[8 * u + 4]]++;
    mapbytes[bytes[8 * u + 5]]++;
    mapbytes[bytes[8 * u + 6]]++;
    mapbytes[bytes[8 * u + 7]]++;
  }
  for (const auto &p : mapbytes) {
    mapping.bytes.emplace(p.second, p.first);
  }
  for (const auto &p : mapwords) {
    mapping.words.emplace(p.second, p.first);
  }
  for (const auto &p : mapdoublewords) {
    mapping.doublewords.emplace(p.second, p.first);
  }
  for (const auto &p : mapquadwords) {
    mapping.quadwords.emplace(p.second, p.first);
  }
  // TODO: sub-byte compression
  // report(PASS, "bytes:");
  // estimate_compression(mapping.bytes);
  // report(PASS, "words:");
  // estimate_compression(mapping.words);
  // report(PASS, "doublewords:");
  // estimate_compression(mapping.doublewords);
  // report(PASS, "quadwords:");
  // estimate_compression(mapping.quadwords);
  // puts("");
  return mapping;
}

size_t estimate_best_size(const multimap_t &mapping) {
  size_t estimates[] = {estimate_compression(mapping.bytes),
                        estimate_compression(mapping.words),
                        estimate_compression(mapping.doublewords),
                        estimate_compression(mapping.quadwords)};
  size_t smallest = *std::min_element(estimates, estimates + 4);
  //  report(INFO, "%lu/", smallest);
  return *std::min_element(estimates, estimates + 4);
}
namespace O2 {
template <> size_t CompressedArray<float>::estimate_compression() {
  // std::sort(mVector.begin(), mVector.end());
  bool useXor = std::is_sorted(mVector.begin(), mVector.end());
  int i;
  int element_count = mVector.size();
  std::vector<uint8_t> sign_bytes((element_count + 7) / 8);
  std::vector<uint8_t> exponent_bytes(element_count);
  uint32_t previous_value = 0;
  // TODO: this part can be done smarter. Split up by bit and the top part will
  // be highly regular, but bottom bits mostly noise. Split up by byte and the
  // top bit is noisier than it should be. 4+8+12 or similar? dynamic?
  std::vector<uint8_t> fraction_bytes_0(element_count);
  std::vector<uint8_t> fraction_bytes_1(element_count);
  std::vector<uint8_t> fraction_bytes_2(element_count);
  for (i = 0; i < element_count - 8; i += 8) {
    uint8_t sign_byte = 0;
    for (int j = 0; j < 8; j++) {
      float value = mVector[i + j];
      uint32_t valueAsUint = *reinterpret_cast<uint32_t *>(&value);
      uint32_t xorValue = useXor ? valueAsUint ^ previous_value : valueAsUint;
      // report(WARN, "%02x ^ %02x = %02x", (valueAsUint >> 23) & 0xFF,
      //        (previous_value >> 23) & 0xFF, (xorValue >> 23) & 0xFF);
      previous_value = valueAsUint;
      // TODO: check and xor per byte?
      bool sign = xorValue >> 31;
      sign_byte |= sign << (7 - j);

      uint8_t exponent = (xorValue >> 23) & 0xFF;
      exponent_bytes[i + j] = exponent;

      uint32_t mantissa = xorValue & 0x007fffff;
      fraction_bytes_0[i + j] = (mantissa >> 0) & 0xFF;
      fraction_bytes_1[i + j] = (mantissa >> 8) & 0xFF;
      fraction_bytes_2[i + j] = (mantissa >> 16) & 0xFF;
      // report(INFO, "%04x", xorValue);
      // report(INFO, "%02x%02x%02x%02x", exponent, fraction_bytes_2[i + j],
      //        fraction_bytes_1[i + j], fraction_bytes_0[i + j]);
    }
    sign_bytes[i / 8] = sign_byte;
    // report(INFO, "\t%02x", sign_byte);
  }
  // report(PASS, "%lu | %lu | %lu, %lu, %lu", sign_bytes.size(),
  //  exponent_bytes.size(), fraction_bytes_0.size(),
  //  fraction_bytes_1.size(), fraction_bytes_2.size());

  multimap_t exponent_mapping = frequency_map(exponent_bytes);
  multimap_t sign_mapping = frequency_map(sign_bytes);
  multimap_t fraction0_mapping = frequency_map(fraction_bytes_0);
  multimap_t fraction1_mapping = frequency_map(fraction_bytes_1);
  multimap_t fraction2_mapping = frequency_map(fraction_bytes_2);
  size_t estimated_size = estimate_best_size(exponent_mapping) +
                          estimate_best_size(sign_mapping) +
                          estimate_best_size(fraction0_mapping) +
                          estimate_best_size(fraction1_mapping) +
                          estimate_best_size(fraction2_mapping);
  report(PASS, "%9lu/%9lu = %.2f", estimated_size, 4 * mVector.size(),
         estimated_size / (4.0 * mVector.size()));
  return estimated_size;
  // report(PASS, "Estimated size: %lu", estimated_size);
  // uint32_t marker = 0xdeadbeef;
  // stream.write((const char *)sign_bytes.data(), sign_bytes.size());
  // stream.write((const char *)(&marker), 4);
  // stream.write((const char *)exponent_bytes.data(), exponent_bytes.size());
  // stream.write((const char *)(&marker), 4);
  // stream.write((const char *)fraction_bytes_0.data(),
  // fraction_bytes_0.size());
  // stream.write((const char *)(&marker), 4);
  // stream.write((const char *)fraction_bytes_1.data(),
  // fraction_bytes_1.size());
  // stream.write((const char *)(&marker), 4);
  // stream.write((const char *)fraction_bytes_2.data(),
  // fraction_bytes_2.size());
}
}
// template <> void CompressedArray<int>::compress_into(std::ostream &stream) {
//   stream.write(reinterpret_cast<const char *>(mVector.data()),
//                mVector.size() * sizeof(int));
// }
