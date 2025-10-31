template <typename Key, typename Val>
HashTableOpen<Key, Val>::HashTableOpen(int i) {
    // TODO
    if (i <= 0) i = 100;                  // if the street size is weird, build a street of 100 houses (safe default)
    M = i;                                // M = how many mailboxes (houses) exist on our street
    ht = new LinkedList<Record>*[M];      // build a row of M hooks, each hook will point to a tiny list (a mailbox's pile)

    for (int idx = 0; idx < M; ++idx) {   // walk down the street, one house at a time
        ht[idx] = new LinkedList<Record>(); // at each house, place an EMPTY little bucket (a linked list ready to hold letters)
    }
}

template <typename Key, typename Val>
HashTableOpen<Key, Val>::HashTableOpen(const HashTableOpen<Key, Val>& copyObj)
: M(0), ht(nullptr) {
    copy(copyObj);
}

template <typename Key, typename Val>
HashTableOpen<Key, Val>& HashTableOpen<Key, Val>::operator=
(const HashTableOpen<Key, Val>& rightObj) {
    if (this != &rightObj) {
        clear();
        copy(rightObj);
    }
    return *this;
}

template <typename Key, typename Val>
HashTableOpen<Key, Val>::~HashTableOpen() {
    // TODO
     if (ht != nullptr) {                          // do we have a street of houses (an array) to clean up?
        for (int i = 0; i < M; ++i) {             // walk down the street, one house (bucket) at a time
            delete ht[i];                         // knock down the tiny mailbox’s pile (delete the linked list object)
            ht[i] = nullptr;                      // leave no dangling pointer string (no ghost rope)
        }
        delete[] ht;                              // finally remove the whole street of hooks (the array itself)
        ht = nullptr;                             // set the street pointer to nothing (no treasure map left)
    }
  
}

template <typename Key, typename Val>
void HashTableOpen<Key, Val>::clear() {
    for (int i = 0; i < M; i++) {
        ht[i]->clear();
    }
}

template <typename Key, typename Val>
void HashTableOpen<Key, Val>::copy(const HashTableOpen<Key, Val>& copyObj) {
    if (M != copyObj.M) {
        LinkedList<Record>** tmp = new LinkedList<Record>*[copyObj.M];
        if (M < copyObj.M) {
            for (int i = 0; i < M; i++) {
                tmp[i] = ht[i];
            }
            for (int i = M; i < copyObj.M; i++) {
                tmp[i] = new LinkedList<Record>;
            }
        }
        else if (M > copyObj.M) {
            for (int i = 0; i < copyObj.M; i++) {
                tmp[i] = ht[i];
            }
            for (int i = copyObj.M; i < M; i++) {
                delete ht[i];
            }
        }

        M = copyObj.M;
        delete[] ht;
        ht = tmp;
    }

    LinkedList<Record>* myList      = nullptr;
    LinkedList<Record>* copyList    = nullptr;
    int                 myListLen   = 0;
    int                 copyListLen = 0;

    for (int i = 0; i < M; i++) {
        myList      = ht[i];
        myListLen   = myList->getLength();
        copyList    = copyObj.ht[i];
        copyListLen = copyList->getLength();

        if (myListLen < copyListLen) {
            for (int j = 0; j < myListLen; j++) {
                myList->replace(j, copyList->getElement(j));
            }
            for (int j = myListLen; j < copyListLen; j++) {
                if (myList->isEmpty()) {
                    myList->append(copyList->getElement(j));
                }
                else {
                    myList->insert(0, copyList->getElement(j));
                }
            }
        }
        else if (myListLen > copyListLen) {
            for (int j = 0; j < copyListLen; j++) {
                myList->replace(j, copyList->getElement(j));
            }
            for (int j = copyListLen; j < myListLen; j++) {
                myList->remove(j);
            }
        }
        else {
            for (int j = 0; j < myListLen; j++) {
                myList->replace(j, copyList->getElement(j));
            }
        }
    }
}

template <typename Key, typename Val>
Val HashTableOpen<Key, Val>::find(const Key& k) const {
    // TODO
     int slot = hash(k);                    // turn the name (key) into a mailbox number: which house on the street?
    const LinkedList<Record>* bucket = ht[slot]; // walk to that house and look at its little mail pile (the linked list)

    int n = bucket->getLength();           // how many letters are in this pile?
    for (int i = 0; i < n; ++i) {          // check each letter one by one
        Record r = bucket->getElement(i);  // pull out the i-th letter to read its label
        if (r.k == k) {                    // does the label (key) match the name we’re looking for?
            return r.v;                    // yes! hand back the message inside the letter (the value)
        }
    }

    // if we checked the whole pile and never found it, the letter isn't here
    throw string("key not found");         // tell the caller: no such letter in this mailbox
}

template <typename Key, typename Val>
int HashTableOpen<Key, Val>::hash(const Key& k) const {
    // how long should each fold be
    // changing this means you should also change the reinterpeted data type
    const int FOLD_LEN = 4;

    // if the fold length is 4, then must treat the string as unsigned numbers
    unsigned* ikey = (unsigned*) k.c_str();

    // calculate how many folds there are
    int ilen = k.length() / FOLD_LEN;

    // accumulator
    unsigned sum = 0;

    // for each fold, now treated as a number, add it to the running total
    for (int i = 0; i < ilen; i++) {
        sum += ikey[i];
    }

    // calculate how many leftover characters there are
    int extra = k.length() - ilen * FOLD_LEN;

    // create a temporary array that will hold those extra characters
    char temp[FOLD_LEN];

    // clear out that array with a 0 value
    ikey    = (unsigned*) temp;
    ikey[0] = 0;

    // copy the extra characters over
    for (int i = 0; i < extra; i++) {
        temp[i] = k[ilen * FOLD_LEN + i];
    }

    // add these final characters as a number to the running total
    sum += ikey[0];

    // calculate the slot position
    int slot = sum % M;

    // display the hashing information
    cout << k << "\thashes to slot " << slot << endl;

    // return the valid slot position
    return slot;
}

template <typename Key, typename Val>
void HashTableOpen<Key, Val>::insert(const Key& k, const Val& v) {
    // TODO
    int slot = hash(k);                          // turn the key into a mailbox number (which house?)
    LinkedList<Record>* bucket = ht[slot];       // walk to that house and look at its little bucket (linked list)

    
    // (Like finding a letter with the same name and swapping the message.)
    int n = bucket->getLength();                 // how many letters are already in this bucket?
    for (int i = 0; i < n; ++i) {                // check each letter in the bucket
        Record r = bucket->getElement(i);        // pull out the i-th letter
        if (r.k == k) {                          // same “name” (key)?
            bucket->replace(i, Record(k, v));    // replace that letter with the new message (value)
            return;                              // done!
        }
    }

    // If not found, just drop a fresh letter at the end. 
    bucket->append(Record(k, v));                // add a new letter (key, value) to this mailbox’s pile
}

template <typename Key, typename Val>
void HashTableOpen<Key, Val>::remove(const Key& k) {
    // TODO
    int slot = hash(k);                          // which mailbox should this letter be in?
    LinkedList<Record>* bucket = ht[slot];       // go to that mailbox’s pile (the linked list)

    int n = bucket->getLength();                 // how many letters are in this pile?
    for (int i = 0; i < n; ++i) {                // scan each letter, one by one
        Record r = bucket->getElement(i);        // peek at the i-th letter
        if (r.k == k) {                          // found the matching name?
            bucket->remove(i);                   // take that letter out of the pile (remove the node)
            return;                              // done!
        }
    }

    // If we finish the scan without finding it, that key doesn't live here.
    throw string("key not found");               // tell the caller we couldn’t find the letter to remove
}

template <typename Key, typename Val>
int HashTableOpen<Key, Val>::size() const {
    // TODO
    int total = 0;                               // start with an empty count (no letters yet)
    for (int i = 0; i < M; ++i) {                // walk down the street, visiting every mailbox
        total += ht[i]->getLength();             // add how many letters are in this house’s pile
    }
    return total;                                // total number of letters (records) across ALL mailboxes
}
