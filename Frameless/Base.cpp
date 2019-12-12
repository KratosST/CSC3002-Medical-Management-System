#ifndef _3002JIEBA_DICT_TRIE_HPP
#define _3002JIEBA_DICT_TRIE_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <limits>
#include <vector>
#include <queue>
#include "limonp/StringUtil.hpp"
#include "limonp/Logging.hpp"
#include "limonp/StdExtension.hpp"
#include "Unicode.hpp"

namespace _3002jieba {

const char* const SPECIAL_SEPARATORS = " \t\n\xEF\xBC\x8C\xE3\x80\x82";
const size_t MAX_WORD_LENGTH = 512;

using namespace limonp;

const double MIN_DOUBLE = -3.14e+100;
const double MAX_DOUBLE = 3.14e+100;
const size_t DICT_COLUMN_NUM = 3;
const char* const UNKNOWN_TAG = "";

    /*
     * Struct a dictionary unit
     * ----------------------
     * Usage: A unit to hold word weight and tag which is exactly same as
     * it is in the utsf file.
     * Example: 抬起 1690 v
     */
    struct DictUnit {
        Unicode word;
        double weight;
        string tag;
    }; // struct DictUnit

    /*
     * Struct a DAG
     * ----------------------
     * Usage: A struct contains one unit and its next position (size_t to *store)
     * Example: address, DictUnit
     */
    struct Dag {
        RuneStr runestr;
        // [offset, nexts.first]
        limonp::LocalVector<pair<size_t, const DictUnit*> > nexts;
        const DictUnit * pInfo;
        double weight;
        size_t nextPos; // TODO
        Dag():runestr(), pInfo(NULL), weight(0.0), nextPos(0) {
        }
    }; // struct Dag

    typedef Rune TrieKey;

    /*
     * Struct a basic data structure part of tree
     * ----------------------
     * Usage: A struct contains two pointers.
     * Example:
     */
    class TrieNode {
        public :
        TrieNode(): next(NULL), ptValue(NULL) {
        }
    public:
        typedef unordered_map<TrieKey, TrieNode*> NextMap;
        NextMap *next;
        const DictUnit *ptValue;
    };

    /*
     * Implement the trie class
     * ----------------------
     * Usage: A Trie is constructed by a group of words and a group
     * of dict units. Those dict units have the same prefix. Trie has
     * a tree structure that can find a dict unit quickly. In each node
     * there is a single character.
     */
    class Trie {
    public:
        Trie(const vector<Unicode>& keys, const vector<const DictUnit*>& valuePointers)
        : root_(new TrieNode) {
            CreateTrie(keys, valuePointers);
        }
        ~Trie() {
            DeleteNode(root_);
        }

        const DictUnit* Find(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end) const {
            if (begin == end) {
                return NULL;
            }

            const TrieNode* ptNode = root_;
            TrieNode::NextMap::const_iterator citer;
            for (RuneStrArray::const_iterator it = begin; it != end; it++) {
                if (NULL == ptNode->next) {
                    return NULL;
                }
                citer = ptNode->next->find(it->rune);
                if (ptNode->next->end() == citer) {
                    return NULL;
                }
                ptNode = citer->second;
            }
            return ptNode->ptValue;
        }

        void Find(RuneStrArray::const_iterator begin,
                  RuneStrArray::const_iterator end,
                  vector<struct Dag>&res,
                  size_t max_word_len = MAX_WORD_LENGTH) const {
            assert(root_ != NULL);
            res.resize(end - begin);

            const TrieNode *ptNode = NULL;
            TrieNode::NextMap::const_iterator citer;
            for (size_t i = 0; i < size_t(end - begin); i++) {
                res[i].runestr = *(begin + i);

                if (root_->next != NULL && root_->next->end() != (citer = root_->next->find(res[i].runestr.rune))) {
                    ptNode = citer->second;
                } else {
                    ptNode = NULL;
                }
                if (ptNode != NULL) {
                    res[i].nexts.push_back(pair<size_t, const DictUnit*>(i, ptNode->ptValue));
                } else {
                    res[i].nexts.push_back(pair<size_t, const DictUnit*>(i, static_cast<const DictUnit*>(NULL)));
                }

                for (size_t j = i + 1; j < size_t(end - begin) && (j - i + 1) <= max_word_len; j++) {
                    if (ptNode == NULL || ptNode->next == NULL) {
                        break;
                    }
                    citer = ptNode->next->find((begin + j)->rune);
                    if (ptNode->next->end() == citer) {
                        break;
                    }
                    ptNode = citer->second;
                    if (NULL != ptNode->ptValue) {
                        res[i].nexts.push_back(pair<size_t, const DictUnit*>(j, ptNode->ptValue));
                    }
                }
            }
        }

        /*
         * Insert a node into the trie
         * ----------------------
         * Usage: Given a unicode word and a dict unit, it goes through
         * every character in this word and decide where the node is.
         */
        void InsertNode(const Unicode& key, const DictUnit* ptValue) {
            if (key.begin() == key.end()) {
                return;
            }

            TrieNode::NextMap::const_iterator kmIter;
            TrieNode *ptNode = root_;
            for (Unicode::const_iterator citer = key.begin(); citer != key.end(); ++citer) {
                if (NULL == ptNode->next) {
                    ptNode->next = new TrieNode::NextMap;
                }
                kmIter = ptNode->next->find(*citer);
                if (ptNode->next->end() == kmIter) {
                    TrieNode *nextNode = new TrieNode;

                    ptNode->next->insert(make_pair(*citer, nextNode));
                    ptNode = nextNode;
                } else {
                    ptNode = kmIter->second;
                }
            }
            assert(ptNode != NULL);
            ptNode->ptValue = ptValue;
        }


    private:
        /*
         * Create(Initialize) a trie
         * ----------------------
         * Usage: Given a group of words and dict units, it inserts every node
         * in oreder to construct the trie tree.
         */
        void CreateTrie(const vector<Unicode>& keys, const vector<const DictUnit*>& valuePointers) {
            if (valuePointers.empty() || keys.empty()) {
                return;
            }
            assert(keys.size() == valuePointers.size());

            for (size_t i = 0; i < keys.size(); i++) {
                InsertNode(keys[i], valuePointers[i]);
            }
        }

        void DeleteNode(TrieNode* node) {
            if (NULL == node) {
                return;
            }
            if (NULL != node->next) {
                for (TrieNode::NextMap::iterator it = node->next->begin(); it != node->next->end(); ++it) {
                    DeleteNode(it->second);
                }
                delete node->next;
            }
            delete node;
        }

        TrieNode* root_;
    }; // class Trie

/*
 * Implement the dict trie class
 * ----------------------
 * Usage: This class is based on the trie class. It allows words
 * in user dictionaries to be loaded.
 */
class DictTrie {

public:
  /*
   * A enum type to choose the weight standard
   * ----------------------
   * Usage: The default value is median.
   */
  enum UserWordWeightOption {
    WordWeightMin,
    WordWeightMedian,
    WordWeightMax,
  }; // enum UserWordWeightOption

  /*
   * Construct a dictionary trie
   * ----------------------
   * Usage: It takes a system dictionary and a user dictionary to build.
   */
  DictTrie(const string& dict_path, const string& user_dict_paths = "", UserWordWeightOption user_word_weight_opt = WordWeightMedian) {
    Init(dict_path, user_dict_paths, user_word_weight_opt);
  }

  ~DictTrie() {
    delete trie_;
  }

  bool InsertUserWord(const string& word, const string& tag = UNKNOWN_TAG) {
    DictUnit node_info;
    if (!MakeNodeInfo(node_info, word, user_word_default_weight_, tag)) {
      return false;
    }
    active_node_infos_.push_back(node_info);
    trie_->InsertNode(node_info.word, &active_node_infos_.back());
    return true;
  }

  /*
   * Insert user-defined words with a frequency parameter
   * ----------------------
   * Usage: If a frequency is given, then weight is calculated by
   * the log of its ratio. Then create the node info and store it.
   * Also insert this node into this trie.
   */
  bool InsertUserWord(const string& word,int freq, const string& tag = UNKNOWN_TAG) {
    DictUnit node_info;
    double weight = freq ? log(1.0 * freq / freq_sum_) : user_word_default_weight_ ;
    if (!MakeNodeInfo(node_info, word, weight , tag)) {
      return false;
    }
    active_node_infos_.push_back(node_info);
    trie_->InsertNode(node_info.word, &active_node_infos_.back());
    return true;
  }

  const DictUnit* Find(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end) const {
    return trie_->Find(begin, end);
  }

  void Find(RuneStrArray::const_iterator begin,
        RuneStrArray::const_iterator end,
        vector<struct Dag>&res,
        size_t max_word_len = MAX_WORD_LENGTH) const {
    trie_->Find(begin, end, res, max_word_len);
  }

  bool Find(const string& word)
  {
    const DictUnit *tmp = NULL;
    RuneStrArray runes;
    if (!DecodeRunesInString(word, runes))
    {
      XLOG(ERROR) << "Decode failed.";
    }
    tmp = Find(runes.begin(), runes.end());
    if (tmp == NULL)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  bool IsUserDictSingleChineseWord(const Rune& word) const {
    return IsIn(user_dict_single_chinese_word_, word);
  }

  double GetMinWeight() const {
    return min_weight_;
  }

  void InserUserDictNode(const string& line) {
    vector<string> buf;
    DictUnit node_info;
    Split(line, buf, " ");
    if(buf.size() == 1){
          MakeNodeInfo(node_info,
                buf[0],
                user_word_default_weight_,
                UNKNOWN_TAG);
        } else if (buf.size() == 2) {
          MakeNodeInfo(node_info,
                buf[0],
                user_word_default_weight_,
                buf[1]);
        } else if (buf.size() == 3) {
          int freq = atoi(buf[1].c_str());
          assert(freq_sum_ > 0.0);
          double weight = log(1.0 * freq / freq_sum_);
          MakeNodeInfo(node_info, buf[0], weight, buf[2]);
        }
        static_node_infos_.push_back(node_info);
        if (node_info.word.size() == 1) {
          user_dict_single_chinese_word_.insert(node_info.word[0]);
        }
  }

  void LoadUserDict(const vector<string>& buf) {
    for (size_t i = 0; i < buf.size(); i++) {
      InserUserDictNode(buf[i]);
    }
  }

  void LoadUserDict(const set<string>& buf) {
    std::set<string>::const_iterator iter;
    for (iter = buf.begin(); iter != buf.end(); iter++){
      InserUserDictNode(*iter);
    }
  }

  void LoadUserDict(const string& filePaths) {
    vector<string> files = limonp::Split(filePaths, "|;");
    size_t lineno = 0;
    for (size_t i = 0; i < files.size(); i++) {
      ifstream ifs(files[i].c_str());
      XCHECK(ifs.is_open()) << "open " << files[i] << " failed";
      string line;

      for (; getline(ifs, line); lineno++) {
        if (line.size() == 0) {
          continue;
        }
        InserUserDictNode(line);
      }
    }
  }

private:
  /*
   * Initialize a dictionary trie
   * ----------------------
   * Usage: It loads two dictionaries (system & user if applicable), set weights
   * and create trie.
   */
  void Init(const string& dict_path, const string& user_dict_paths, UserWordWeightOption user_word_weight_opt) {
    LoadDict(dict_path);
    freq_sum_ = CalcFreqSum(static_node_infos_); // static_node_infos_: a vector of dictionary units
    CalculateWeight(static_node_infos_, freq_sum_);
    SetStaticWordWeights(user_word_weight_opt);

    if (user_dict_paths.size()) {
      LoadUserDict(user_dict_paths);
    }
    Shrink(static_node_infos_);
    CreateTrie(static_node_infos_);
  }

  void CreateTrie(const vector<DictUnit>& dictUnits) {
    assert(dictUnits.size());
    vector<Unicode> words;
    vector<const DictUnit*> valuePointers;
    for (size_t i = 0 ; i < dictUnits.size(); i ++) {
      words.push_back(dictUnits[i].word);
      valuePointers.push_back(&dictUnits[i]);
    }

    trie_ = new Trie(words, valuePointers);
  }

  /*
   * Decode information to create a dict unit
   * ----------------------
   * Usage: It examines information in a line and assign them into a dict unit.
   */
  bool MakeNodeInfo(DictUnit& node_info,
        const string& word,
        double weight,
        const string& tag) {
    if (!DecodeRunesInString(word, node_info.word)) {
      XLOG(ERROR) << "Decode " << word << " failed.";
      return false;
    }
    node_info.weight = weight;
    node_info.tag = tag;
    return true;
  }

  /*
   * Load a dictionary (utf8 file)
   * ----------------------
   * Usage: It reads a dict file, extract each node and
   * store it in a dict unit vector "static_node_infos_".
   */
  void LoadDict(const string& filePath) {
    ifstream ifs(filePath.c_str());
    XCHECK(ifs.is_open()) << "open " << filePath << " failed.";
    string line;
    vector<string> buf;

    DictUnit node_info;
    for (size_t lineno = 0; getline(ifs, line); lineno++) {
      Split(line, buf, " ");
      XCHECK(buf.size() == DICT_COLUMN_NUM) << "split result illegal, line:" << line;
      MakeNodeInfo(node_info,
            buf[0],
            atof(buf[1].c_str()),
            buf[2]);
      static_node_infos_.push_back(node_info);
    }
  }

  static bool WeightCompare(const DictUnit& lhs, const DictUnit& rhs) {
    return lhs.weight < rhs.weight;
  }

  void SetStaticWordWeights(UserWordWeightOption option) {
    XCHECK(!static_node_infos_.empty());
    vector<DictUnit> x = static_node_infos_;
    sort(x.begin(), x.end(), WeightCompare);
    min_weight_ = x[0].weight;
    max_weight_ = x[x.size() - 1].weight;
    median_weight_ = x[x.size() / 2].weight;
    switch (option) {
     case WordWeightMin:
       user_word_default_weight_ = min_weight_;
       break;
     case WordWeightMedian:
       user_word_default_weight_ = median_weight_;
       break;
     default:
       user_word_default_weight_ = max_weight_;
       break;
    }
  }

  double CalcFreqSum(const vector<DictUnit>& node_infos) const {
    double sum = 0.0;
    for (size_t i = 0; i < node_infos.size(); i++) {
      sum += node_infos[i].weight;
    }
    return sum;
  }

  void CalculateWeight(vector<DictUnit>& node_infos, double sum) const {
    assert(sum > 0.0);
    for (size_t i = 0; i < node_infos.size(); i++) {
      DictUnit& node_info = node_infos[i];
      assert(node_info.weight > 0.0);
      node_info.weight = log(double(node_info.weight)/sum);
    }
  }

  void Shrink(vector<DictUnit>& units) const {
    vector<DictUnit>(units.begin(), units.end()).swap(units);
  }

  vector<DictUnit> static_node_infos_;
  deque<DictUnit> active_node_infos_; // must not be vector
  Trie * trie_;

  double freq_sum_;
  double min_weight_;
  double max_weight_;
  double median_weight_;
  double user_word_default_weight_;
  unordered_set<Rune> user_dict_single_chinese_word_;
};


/*
 * Implement the superclass of different models
 * ----------------------
 * Usage: This the superclass of hmm, mp, mix, and query segments. It has
 * a virtual method cut that will apply in every subclass.
 *
 */
class SegmentBase {
    public:
        SegmentBase() {
            XCHECK(ResetSeparators(SPECIAL_SEPARATORS));
        }
        virtual ~SegmentBase() {
        }

        virtual void Cut(const string& sentence, vector<string>& words) const = 0;

        bool ResetSeparators(const string& s) {
            symbols_.clear();
            RuneStrArray runes;
            if (!DecodeRunesInString(s, runes)) {
                XLOG(ERROR) << "decode " << s << " failed";
                return false;
            }
            for (size_t i = 0; i < runes.size(); i++) {
                if (!symbols_.insert(runes[i].rune).second) {
                    XLOG(ERROR) << s.substr(runes[i].offset, runes[i].len) << " already exists";
                    return false;
                }
            }
            return true;
        }
    protected:
        unordered_set<Rune> symbols_;
    }; // class SegmentBase

/*
 * A pre-processing method for cut method
 * ----------------------
 * Usage: It turns a string input into a unicode type, and cut it based
 * on a group of Rune characters (symbols_). There will be many segments
 * that need to be processed in each model.
 */
class PreFilter {
    public:
        //TODO use WordRange instead of Range
        struct Range {
            RuneStrArray::const_iterator begin;
            RuneStrArray::const_iterator end;
        }; // struct Range

        PreFilter(const unordered_set<Rune>& symbols,
                  const string& sentence)
        : symbols_(symbols) {
            if (!DecodeRunesInString(sentence, sentence_)) {
                XLOG(ERROR) << "decode failed. ";
            }
            cursor_ = sentence_.begin();
        }
        ~PreFilter() {
        }
        bool HasNext() const {
            return cursor_ != sentence_.end();
        }
        Range Next() {
            Range range;
            range.begin = cursor_;
            while (cursor_ != sentence_.end()) {
                if (IsIn(symbols_, cursor_->rune)) {
                    if (range.begin == cursor_) {
                        cursor_ ++;
                    }
                    range.end = cursor_;
                    return range;
                }
                cursor_ ++;
            }
            range.end = sentence_.end();
            return range;
        }
    private:
        RuneStrArray::const_iterator cursor_;
        RuneStrArray sentence_;
        const unordered_set<Rune>& symbols_;
    }; // class PreFilter


class SegmentTagged : public SegmentBase{
    public:
        SegmentTagged() {
        }
        virtual ~SegmentTagged() {
        }

        virtual bool Tag(const string& src, vector<pair<string, string> >& res) const = 0;

        virtual const DictTrie* GetDictTrie() const = 0;

    }; // class SegmentTagged

}

#endif
