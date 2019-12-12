#ifndef _3002JIEBA_MPSEGMENT_H
#define _3002JIEBA_MPSEGMENT_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <cassert>
#include <memory.h>
#include "limonp/Logging.hpp"
#include "Base.cpp"
#include "HMMModel.hpp"

namespace _3002jieba {
    /*
     * Construct Class HMMSegment
     * ----------------------
     * Usage:To Get HMM segment result
     */

    //Inheritance from Segmentbase
    class HMMSegment: public SegmentBase {
    public:
        HMMSegment(const string& filePath)
        : model_(new HMMModel(filePath)), isNeedDestroy_(true) {
        }
        HMMSegment(const HMMModel* model)
        : model_(model), isNeedDestroy_(false) {
        }
        ~HMMSegment() {
            if (isNeedDestroy_) {
                delete model_;
            }
        }
        //Overloading Functions: Cut
        void Cut(const string& sentence,
                 vector<string>& words) const {
            vector<Word> tmp;
            Cut(sentence, tmp);
            GetStringsFromWords(tmp, words);
        }
        void Cut(const string& sentence,
                 vector<Word>& words) const {
            PreFilter pre_filter(symbols_, sentence);
            PreFilter::Range range;
            vector<WordRange> wrs;
            wrs.reserve(sentence.size()/2);
            while (pre_filter.HasNext()) {
                range = pre_filter.Next();
                Cut(range.begin, range.end, wrs);
            }
            words.clear();
            words.reserve(wrs.size());
            GetWordsFromWordRanges(sentence, wrs, words);
        }
        void Cut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res) const {
            RuneStrArray::const_iterator left = begin;
            RuneStrArray::const_iterator right = begin;
            while (right != end) {
                if (right->rune < 0x80) {
                    if (left != right) {
                        InternalCut(left, right, res);
                    }
                    left = right;
                    do {
                        right = SequentialLetterRule(left, end);
                        if (right != left) {
                            break;
                        }
                        right = NumbersRule(left, end);
                        if (right != left) {
                            break;
                        }
                        right ++;
                    } while (false);
                    WordRange wr(left, right - 1);
                    res.push_back(wr);
                    left = right;
                } else {
                    right++;
                }
            }
            if (left != right) {
                InternalCut(left, right, res);
            }
        }
    private:
        // sequential letters rule
        RuneStrArray::const_iterator SequentialLetterRule(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end) const {
            Rune x = begin->rune;
            if (('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z')) {
                begin ++;
            } else {
                return begin;
            }
            while (begin != end) {
                x = begin->rune;
                if (('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z') || ('0' <= x && x <= '9')) {
                    begin ++;
                } else {
                    break;
                }
            }
            return begin;
        }
        //Number rule
        RuneStrArray::const_iterator NumbersRule(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end) const {
            Rune x = begin->rune;
            if ('0' <= x && x <= '9') {
                begin ++;
            } else {
                return begin;
            }
            while (begin != end) {
                x = begin->rune;
                if ( ('0' <= x && x <= '9') || x == '.') {
                    begin++;
                } else {
                    break;
                }
            }
            return begin;
        }
        //Internally Cut by Viterbi HMM
        void InternalCut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res) const {
            vector<size_t> status;
            Viterbi(begin, end, status);

            RuneStrArray::const_iterator left = begin;
            RuneStrArray::const_iterator right;
            for (size_t i = 0; i < status.size(); i++) {
                if (status[i] % 2) { //if (HMMModel::E == status[i] || HMMModel::S == status[i])
                    right = begin + i + 1;
                    WordRange wr(left, right - 1);
                    res.push_back(wr);
                    left = right;
                }
            }
        }
        //Exactly same in the HMM no need to bring it agian...
        void Viterbi(RuneStrArray::const_iterator begin,
                     RuneStrArray::const_iterator end,
                     vector<size_t>& status) const {
            size_t Y = HMMModel::STATUS_SUM;
            size_t X = end - begin;

            size_t XYSize = X * Y;
            size_t now, old, stat;
            double tmp, endE, endS;

            vector<int> path(XYSize);
            vector<double> weight(XYSize);

            //start
            for (size_t y = 0; y < Y; y++) {
                weight[0 + y * X] = model_->startProb[y] + model_->GetEmitProb(model_->emitProbVec[y], begin->rune, MIN_DOUBLE);
                path[0 + y * X] = -1;
            }

            double emitProb;
            //Find the best!!!
            for (size_t x = 1; x < X; x++) {
                for (size_t y = 0; y < Y; y++) {
                    now = x + y*X;
                    weight[now] = MIN_DOUBLE;
                    path[now] = HMMModel::E; // warning
                    emitProb = model_->GetEmitProb(model_->emitProbVec[y], (begin+x)->rune, MIN_DOUBLE);
                    for (size_t preY = 0; preY < Y; preY++) {
                        old = x - 1 + preY * X;
                        tmp = weight[old] + model_->transProb[preY][y] + emitProb;
                        if (tmp > weight[now]) {
                            weight[now] = tmp;
                            path[now] = preY;
                        }
                    }
                }
            }

            endE = weight[X-1+HMMModel::E*X];
            endS = weight[X-1+HMMModel::S*X];
            stat = 0;
            //S or E
            if (endE >= endS) {
                stat = HMMModel::E;
            } else {
                stat = HMMModel::S;
            }

            status.resize(X);
            for (int x = X -1 ; x >= 0; x--) {
                status[x] = stat;
                stat = path[x + stat*X];
            }
        }

        const HMMModel* model_;
        bool isNeedDestroy_;
    }; // class HMMSegment

    
    /*
     * Construct Class MPSegment
     * ----------------------
     * Usage:To Get MP segment result
     */
class MPSegment: public SegmentTagged {
 public:
    //If we have the UserDictPath
  MPSegment(const string& dictPath, const string& userDictPath = "")
    : dictTrie_(new DictTrie(dictPath, userDictPath)), isNeedDestroy_(true) {
  }
    //Overloading
  MPSegment(const DictTrie* dictTrie)
    : dictTrie_(dictTrie), isNeedDestroy_(false) {
    assert(dictTrie_);
  }
  ~MPSegment() {
    if (isNeedDestroy_) {
        //Free Memory Space
      delete dictTrie_;
    }
  }

  void Cut(const string& sentence, vector<string>& words) const {
    Cut(sentence, words, MAX_WORD_LENGTH);
  }
    //Overloading Functions:
  void Cut(const string& sentence,
        vector<string>& words,
        size_t max_word_len) const {
    vector<Word> tmp;
    Cut(sentence, tmp, max_word_len);
    GetStringsFromWords(tmp, words);
  }
  void Cut(const string& sentence,
        vector<Word>& words,
        size_t max_word_len = MAX_WORD_LENGTH) const {
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    vector<WordRange> wrs;
    wrs.reserve(sentence.size()/2);
    while (pre_filter.HasNext()) {
      range = pre_filter.Next();
      Cut(range.begin, range.end, wrs, max_word_len);
    }
    words.clear();
    words.reserve(wrs.size());
    GetWordsFromWordRanges(sentence, wrs, words);
  }
  void Cut(RuneStrArray::const_iterator begin,
           RuneStrArray::const_iterator end,
           vector<WordRange>& words,
           size_t max_word_len = MAX_WORD_LENGTH) const {
    vector<Dag> dags;
    dictTrie_->Find(begin,
          end,
          dags,
          max_word_len);
      //Defined in the end...
    CalcDP(dags);
    CutByDag(begin, end, dags, words);
  }

  const DictTrie* GetDictTrie() const {
    return dictTrie_;
  }

  bool Tag(const string& src, vector<pair<string, string> >& res) const {
    return tagger_.Tag(src, res, *this);
  }

  bool IsUserDictSingleChineseWord(const Rune& value) const {
    return dictTrie_->IsUserDictSingleChineseWord(value);
  }
 private:
    /*
     * Function: CalcDP
     * ----------------------
     * Calculate the Probabilities of different CUT to find the best.
     */

  void CalcDP(vector<Dag>& dags) const {
    size_t nextPos;
    const DictUnit* p;
    double val;

    for (vector<Dag>::reverse_iterator rit = dags.rbegin(); rit != dags.rend(); rit++) {
      rit->pInfo = NULL;
      rit->weight = MIN_DOUBLE;
      assert(!rit->nexts.empty());
        //Traverse
      for (LocalVector<pair<size_t, const DictUnit*> >::const_iterator it = rit->nexts.begin(); it != rit->nexts.end(); it++) {
        nextPos = it->first;
          //Probability
        p = it->second;
        val = 0.0;
        if (nextPos + 1 < dags.size()) {
          val += dags[nextPos + 1].weight;
        }

        if (p) {
          val += p->weight;
        } else {
          val += dictTrie_->GetMinWeight();//Built-in the tree
        }
        if (val > rit->weight) {
          rit->pInfo = p;
          rit->weight = val;
        }
      }
    }
  }
    //Core!!
    /*
     * Function: CutByDAG
     * ----------------------
     * Usage:To cut by DAG Graph best
     * Example: CutByDag(begin, end, dags, words);
     */

  void CutByDag(RuneStrArray::const_iterator begin,
        RuneStrArray::const_iterator end,
        const vector<Dag>& dags,
        vector<WordRange>& words) const {
    size_t i = 0;
    while (i < dags.size()) {
      const DictUnit* p = dags[i].pInfo;
      if (p) {
        assert(p->word.size() >= 1);
        WordRange wr(begin + i, begin + i + p->word.size() - 1);
        words.push_back(wr);
        i += p->word.size();
      } else { //single chinese word
        WordRange wr(begin + i, begin + i);
        words.push_back(wr);
        i++;
      }
    }
  }

  const DictTrie* dictTrie_;
  bool isNeedDestroy_;
  PosTagger tagger_;

}; // class MPSegment

} // namespace _3002jieba

#endif
