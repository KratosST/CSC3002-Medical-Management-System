#ifndef _3002JIEAB_JIEBA_H
#define _3002JIEAB_JIEBA_H

#include "QuerySegment.hpp"

namespace _3002jieba {
    
    /*
     * Jieba class
     * ----------------------
     * Usage: an overall class contains trie struct, models, segmentation methods.
     * It contains several cut/tag methods using single/multiple models from
     * mix, hmm, mp and query.
     * Example:
     * _3002jieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH);
     * vector<pair<string, string>> tagres;
     * jieba.Tag(symp, tagres);
     */
    class Jieba {

    public:
        // constructor
        Jieba(const string& dict_path,
              const string& model_path,
              const string& user_dict_path)
        : dict_trie_(dict_path, user_dict_path),
        model_(model_path),
        mp_seg_(&dict_trie_),
        hmm_seg_(&model_),
        mix_seg_(&dict_trie_, &model_),
        query_seg_(&dict_trie_, &model_)
        {}
        
        // destructor
        ~Jieba() {}
        
        void Cut(const string& sentence, vector<string>& words, bool hmm = true) const {
            mix_seg_.Cut(sentence, words, hmm);
        }
        void Cut(const string& sentence, vector<Word>& words, bool hmm = true) const {
            mix_seg_.Cut(sentence, words, hmm);
        }

//        void CutForSearch(const string& sentence, vector<string>& words, bool hmm = true) const {
//            query_seg_.Cut(sentence, words, hmm);
//        }
//        void CutForSearch(const string& sentence, vector<Word>& words, bool hmm = true) const {
//            query_seg_.Cut(sentence, words, hmm);
//        }
        
        void CutHMM(const string& sentence, vector<string>& words) const {
            hmm_seg_.Cut(sentence, words);
        }
        void CutHMM(const string& sentence, vector<Word>& words) const {
            hmm_seg_.Cut(sentence, words);
        }
        void CutSmall(const string& sentence, vector<string>& words, size_t max_word_len) const {
            mp_seg_.Cut(sentence, words, max_word_len);
        }
        void CutSmall(const string& sentence, vector<Word>& words, size_t max_word_len) const {
            mp_seg_.Cut(sentence, words, max_word_len);
        }
        
        /*
         * Implementation of Tag Function
         * ----------------------
         * Usage: Our kernel function using mix segmentation model. It calls
         * the Tag function of a PosTagger class in HMMModel.hpp.
         */
        void Tag(const string& sentence, vector<pair<string, string> >& words) const {
            mix_seg_.Tag(sentence, words);
        }
        
        string LookupTag(const string &str) const {
            return mix_seg_.LookupTag(str);
        }
//        bool InsertUserWord(const string& word, const string& tag = UNKNOWN_TAG) {
//            return dict_trie_.InsertUserWord(word, tag);
//        }
        
//        bool InsertUserWord(const string& word,int freq, const string& tag = UNKNOWN_TAG) {
//            return dict_trie_.InsertUserWord(word,freq, tag);
//        }
        
        bool Find(const string& word)
        {
            return dict_trie_.Find(word);
        }
        
        void ResetSeparators(const string& s) {
            //TODO
            mp_seg_.ResetSeparators(s);
            hmm_seg_.ResetSeparators(s);
            mix_seg_.ResetSeparators(s);
            query_seg_.ResetSeparators(s);
        }
        
        const DictTrie* GetDictTrie() const {
            return &dict_trie_;
        }
        
        const HMMModel* GetHMMModel() const {
            return &model_;
        }
        
        void LoadUserDict(const vector<string>& buf)  {
            dict_trie_.LoadUserDict(buf);
        }
        
        void LoadUserDict(const set<string>& buf)  {
            dict_trie_.LoadUserDict(buf);
        }
        
        void LoadUserDict(const string& path)  {
            dict_trie_.LoadUserDict(path);
        }
        
    private:
        DictTrie dict_trie_;
        HMMModel model_;
        
        // They share the same dict trie and model
        MPSegment mp_seg_;
        HMMSegment hmm_seg_;
        MixSegment mix_seg_;
        QuerySegment query_seg_;

    }; // class Jieba
    
} // namespace _3002jieba


#endif
