#ifndef _3002JIEBA_QUERYSEGMENT_H
#define _3002JIEBA_QUERYSEGMENT_H

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "Unicode.hpp"
#include "Base.cpp"
#include "MixSegment.hpp"

using namespace std;

namespace _3002jieba {
    //MP first then HMM
    class MixSegment: public SegmentTagged {
    public:
        MixSegment(const string& mpSegDict, const string& hmmSegDict,
                   const string& userDict = "")
        : mpSeg_(mpSegDict, userDict),
        hmmSeg_(hmmSegDict) {
        }
        MixSegment(const DictTrie* dictTrie, const HMMModel* model)
        : mpSeg_(dictTrie), hmmSeg_(model) {
        }
        ~MixSegment() {
        }

        void Cut(const string& sentence, vector<string>& words) const {
            Cut(sentence, words, true);
        }
        void Cut(const string& sentence, vector<string>& words, bool hmm) const {
            vector<Word> tmp;
            Cut(sentence, tmp, hmm);
            GetStringsFromWords(tmp, words);
        }
        void Cut(const string& sentence, vector<Word>& words, bool hmm = true) const {
            PreFilter pre_filter(symbols_, sentence);
            PreFilter::Range range;
            vector<WordRange> wrs;
            wrs.reserve(sentence.size() / 2);
            while (pre_filter.HasNext()) {
                range = pre_filter.Next();
                Cut(range.begin, range.end, wrs, hmm);
            }
            words.clear();
            words.reserve(wrs.size());
            GetWordsFromWordRanges(sentence, wrs, words);
        }

        void Cut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res, bool hmm) const {
            if (!hmm) {
                mpSeg_.Cut(begin, end, res);
                return;
            }
            vector<WordRange> words;
            assert(end >= begin);
            words.reserve(end - begin);
            mpSeg_.Cut(begin, end, words);

            vector<WordRange> hmmRes;
            hmmRes.reserve(end - begin);
            for (size_t i = 0; i < words.size(); i++) {
                //if mp Get a word, it's ok, put it into result
                if (words[i].left != words[i].right || (words[i].left == words[i].right && mpSeg_.IsUserDictSingleChineseWord(words[i].left->rune))) {
                    res.push_back(words[i]);
                    continue;
                }

                // if mp Get a single one and it is not in userdict, collect it in sequence
                size_t j = i;
                while (j < words.size() && words[j].left == words[j].right && !mpSeg_.IsUserDictSingleChineseWord(words[j].left->rune)) {
                    j++;
                }

                // Cut the sequence with hmm
                assert(j - 1 >= i);
                // TODO
                hmmSeg_.Cut(words[i].left, words[j - 1].left + 1, hmmRes);
                //put hmm result to result
                for (size_t k = 0; k < hmmRes.size(); k++) {
                    res.push_back(hmmRes[k]);
                }

                //clear tmp vars
                hmmRes.clear();

                //let i jump over this piece
                i = j - 1;
            }
        }

        const DictTrie* GetDictTrie() const {
            return mpSeg_.GetDictTrie();
        }

        bool Tag(const string& src, vector<pair<string, string> >& res) const {
            return tagger_.Tag(src, res, *this);
        }

        string LookupTag(const string &str) const {
            return tagger_.LookupTag(str, *this);
        }

    private:
        MPSegment mpSeg_;
        HMMSegment hmmSeg_;
        PosTagger tagger_;

    }; // class MixSegment

class QuerySegment: public SegmentBase {
 public:
  QuerySegment(const string& dict, const string& model, const string& userDict = "")
    : mixSeg_(dict, model, userDict),
      trie_(mixSeg_.GetDictTrie()) {
  }
  QuerySegment(const DictTrie* dictTrie, const HMMModel* model)
    : mixSeg_(dictTrie, model), trie_(dictTrie) {
  }
  ~QuerySegment() {
  }

  void Cut(const string& sentence, vector<string>& words) const {}

  void Cut(const string& sentence, vector<string>& words, bool hmm) const {}

  void Cut(const string& sentence, vector<Word>& words, bool hmm = true) const {}

  void Cut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res, bool hmm) const {
    //use mix Cut first
    vector<WordRange> mixRes;
    mixSeg_.Cut(begin, end, mixRes, hmm);

    vector<WordRange> fullRes;
    for (vector<WordRange>::const_iterator mixResItr = mixRes.begin(); mixResItr != mixRes.end(); mixResItr++) {
      if (mixResItr->Length() > 2) {
        for (size_t i = 0; i + 1 < mixResItr->Length(); i++) {
          WordRange wr(mixResItr->left + i, mixResItr->left + i + 1);
          if (trie_->Find(wr.left, wr.right + 1) != NULL) {
            res.push_back(wr);
          }
        }
      }
      if (mixResItr->Length() > 3) {
        for (size_t i = 0; i + 2 < mixResItr->Length(); i++) {
          WordRange wr(mixResItr->left + i, mixResItr->left + i + 2);
          if (trie_->Find(wr.left, wr.right + 1) != NULL) {
            res.push_back(wr);
          }
        }
      }
      res.push_back(*mixResItr);
    }
  }
 private:
  bool IsAllAscii(const Unicode& s) const {
   for(size_t i = 0; i < s.size(); i++) {
     if (s[i] >= 0x80) {
       return false;
     }
   }
   return true;
  }

    // MixSegment class
    MixSegment mixSeg_;

    const DictTrie* trie_;
}; // QuerySegment

} // namespace cppjieba

string Approximation(string input)
{
    int flag=0;
    string output;
    if(input=="拉肚子"||input=="拉了一整天"||input=="拉稀"||input=="拉了好多次肚子"){
        output="腹泻";
        flag=1;
    }
    if(input=="肚子痛"||input=="肚子疼"){
        output="腹部疼痛";
        flag=1;
    }
    if(input=="嗓子疼"||input=="嗓子痛"||input=="喉咙疼"||input=="咽喉痛"){
        output="喉咙痛";
        flag=1;
    }
    if(input=="疲劳"||input=="疲惫"||input=="想睡觉"||input=="身体被掏空"){
        output="困";
        flag=1;
    }
    if(input=="头痛"||input=="脑阔疼"||input=="脑阔痛"){
        output="头疼";
        flag=1;
    }
    if(input=="出汗"||input=="出了很多汗"||input=="出了好多汗") {
        output="出汗增加";
        flag=1;
    }
    if(input=="睡不着觉"||input=="没睡着") {
        output="失眠";
        flag=1;
    }
    if(flag==0){
        output=input;
    }
    return output;
}

#endif
