#ifndef _3002JIEBA_HMMMODEL_H
#define _3002JIEBA_HMMMODEL_H

#include "limonp/StringUtil.hpp"
#include "Base.cpp"

namespace _3002jieba {

using namespace limonp;
typedef unordered_map<Rune, double> EmitProbMap;

    static const char* const POS_M = "m";
    static const char* const POS_ENG = "eng";
    static const char* const POS_X = "x";

    /*
     * Struct HMM
     * ----------------------
     * Usage: A unit to defined states B:Begining M:Middle E:Ending S:Single word Status_Sum to hold all
     * with several methods to load and output the most possible outcome
     * Example: Weight[0][0] The first word with B's probability
     */
struct HMMModel {
  /*
   * STATUS:
   * 0: HMMModel::B, 1: HMMModel::E, 2: HMMModel::M, 3:HMMModel::S
   * */
  enum {B = 0, E = 1, M = 2, S = 3, STATUS_SUM = 4};

  HMMModel(const string& modelPath) {
    //Set memory space for processing
    memset(startProb, 0, sizeof(startProb));
    memset(transProb, 0, sizeof(transProb));
    statMap[0] = 'B';
    statMap[1] = 'E';
    statMap[2] = 'M';
    statMap[3] = 'S';
    //Achieve the processed probabilities
    emitProbVec.push_back(&emitProbB);
    emitProbVec.push_back(&emitProbE);
    emitProbVec.push_back(&emitProbM);
    emitProbVec.push_back(&emitProbS);
    LoadModel(modelPath);
  }
  ~HMMModel() {
  }
    /*
     * Function: LoadModel
     * ----------------------
     * Usage:Open a file and initialize the HMM model
     */
  void LoadModel(const string& filePath) {
    ifstream ifile(filePath.c_str());
    XCHECK(ifile.is_open()) << "open " << filePath << " failed";
    string line;
    vector<string> tmp;
    vector<string> tmp2;
    //Load startProb
    XCHECK(GetLine(ifile, line));
    Split(line, tmp, " ");
    XCHECK(tmp.size() == STATUS_SUM);
    for (size_t j = 0; j< tmp.size(); j++) {
      startProb[j] = atof(tmp[j].c_str());
    }

    //Load transProb
    for (size_t i = 0; i < STATUS_SUM; i++) {
      XCHECK(GetLine(ifile, line));
      Split(line, tmp, " ");
      XCHECK(tmp.size() == STATUS_SUM);
      for (size_t j =0; j < STATUS_SUM; j++) {
        transProb[i][j] = atof(tmp[j].c_str());
      }
    }

    //Load emitProbB
    XCHECK(GetLine(ifile, line));
    XCHECK(LoadEmitProb(line, emitProbB));

    //Load emitProbE
    XCHECK(GetLine(ifile, line));
    XCHECK(LoadEmitProb(line, emitProbE));

    //Load emitProbM
    XCHECK(GetLine(ifile, line));
    XCHECK(LoadEmitProb(line, emitProbM));

    //Load emitProbS
    XCHECK(GetLine(ifile, line));
    XCHECK(LoadEmitProb(line, emitProbS));
  }
    /*
     * Function: GetEmitProb
     * ----------------------
     * Usage:Utilize the iterator to get every probability contained in base data structure-Tree
     */
  double GetEmitProb(const EmitProbMap* ptMp, Rune key,
        double defVal)const {
    EmitProbMap::const_iterator cit = ptMp->find(key);
    if (cit == ptMp->end()) {
      return defVal;
    }
    return cit->second;
  }
    //Simple GetLine Definition to get rid of importing Standford Library
  bool GetLine(ifstream& ifile, string& line) {
    while (getline(ifile, line)) {
      Trim(line);
      if (line.empty()) {
        continue;
      }
      if (StartsWith(line, "#")) {
        continue;
      }
      return true;
    }
    return false;
  }
    //Get above functions to fully run
  bool LoadEmitProb(const string& line, EmitProbMap& mp) {
    if (line.empty()) {
      return false;
    }
    vector<string> tmp, tmp2;
    Unicode unicode;
    Split(line, tmp, ",");
    for (size_t i = 0; i < tmp.size(); i++) {
      Split(tmp[i], tmp2, ":");
      if (2 != tmp2.size()) {
        XLOG(ERROR) << "emitProb illegal.";
        return false;
      }
      if (!DecodeRunesInString(tmp2[0], unicode) || unicode.size() != 1) {
        XLOG(ERROR) << "TransCode failed.";
        return false;
      }
      mp[unicode[0]] = atof(tmp2[1].c_str());
    }
    return true;
  }

  char statMap[STATUS_SUM];
  double startProb[STATUS_SUM];
  double transProb[STATUS_SUM][STATUS_SUM];
  EmitProbMap emitProbB;
  EmitProbMap emitProbE;
  EmitProbMap emitProbM;
  EmitProbMap emitProbS;
  vector<EmitProbMap* > emitProbVec;
}; // struct HMMModel

    
    /*
     * Construct Class PosTagger
     * ----------------------
     * Usage:To Get tag from the dictionary Given!! the cutted result
     */
    class PosTagger {
    public:
        //Constructor
        PosTagger() {
        }
        //Deconstructor
        ~PosTagger() {
        }
        /*
         * Implementation section
         * ----------------------
         * C++ requires that the implementation for a template class be available
         * to the compiler whenever that type is used.  The effect of this
         * restriction is that header files must include the implementation.
         * Clients should not need to look at any of the code beyond this point.
         */

        bool Tag(const string& src, vector<pair<string, string> >& res, const SegmentTagged& segment) const {
            vector<string> CutRes;
            segment.Cut(src, CutRes);
            //Utilize vector to hold the result
            for (vector<string>::iterator itr = CutRes.begin(); itr != CutRes.end(); ++itr) {
                res.push_back(make_pair(*itr, LookupTag(*itr, segment)));
            }
            return !res.empty();
        }
        //Get the tagger from dictionary
        string LookupTag(const string &str, const SegmentTagged& segment) const {
            const DictUnit *tmp = NULL;
            RuneStrArray runes;
            const DictTrie * dict = segment.GetDictTrie();
            assert(dict != NULL);
            if (!DecodeRunesInString(str, runes)) {
                XLOG(ERROR) << "Decode failed.";
                return POS_X;
            }
            tmp = dict->Find(runes.begin(), runes.end());
            if (tmp == NULL || tmp->tag.empty()) {
                return SpecialRule(runes);
            } else {
                return tmp->tag;
            }
        }
    //Private Part
    private:
        const char* SpecialRule(const RuneStrArray& unicode) const {
            size_t m = 0;
            size_t eng = 0;
            for (size_t i = 0; i < unicode.size() && eng < unicode.size() / 2; i++) {
                if (unicode[i].rune < 0x80) {
                    eng ++;
                    if ('0' <= unicode[i].rune && unicode[i].rune <= '9') {
                        m++;
                    }
                }
            }
            // ascii char is not found
            if (eng == 0) {
                return POS_X;
            }
            // all the ascii is number char
            if (m == eng) {
                return POS_M;
            }
            // the ascii chars contain english letter
            return POS_ENG;
        }

    }; // class PosTagger

} // namespace _3002jieba

#endif
