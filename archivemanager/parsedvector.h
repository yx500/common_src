#ifndef PARSEDVECTOR_H
#define PARSEDVECTOR_H

#include <QVector>
#include <QMap>
template <class T_ind,class K ,class T_data>
class ParsedVector
{
public:
    struct tid{
        tid(){pos_in_index=0;data=0;}
        tid(int _pos_in_index,T_data _data){pos_in_index=_pos_in_index;data=_data;}
        int pos_in_index;
        T_data data;
    };
    ParsedVector(){}
    void add(T_ind index_val,K k,T_data data)
    {
        bool ex=false;
        int ind_pos=q2find_ind_pos(index_val,ex);
        if (!ex) {
            ind_pos++;
            lIndex.insert(ind_pos,index_val);
        }
        // добавляем к строке
        if (!mRows.contains(k)){
            QList<tid> v;
            mRows.insert(k,v);
        }
        QList<tid> &vR=mRows[k];
        tid t(ind_pos,data);
        if ((vR.isEmpty())||(vR.back().pos_in_index<=ind_pos)){
            vR.push_back(t);
        } else {
            int r_i=q2findpos(ind_pos,vR);
            if (r_i>=0)
                vR.insert(r_i,t); else
                vR.insert(0,t);
        }
        //vR.push_back(data_pos);??
    }

    QMap<K ,T_data> srez_data(int ind_pos)
    {
        QMap<K, T_data> r;
        for (int i=0;i<mRows.keys().size();i++) {
            K k=mRows.keys().at(i);
            QList<tid> &vR=mRows[k];
            int r_i=q2findpos(ind_pos,vR);
            if (r_i>=0)
                r.insert(k,vR[r_i].data); else
                r.insert(k,-1);
        }
        return r;
    }

    struct tsrezi{
        tsrezi(K _k,T_data _data){k=_k;data=_data;}
        K k;
        T_data data;
    };

    QList<tsrezi> &srez_data_l(int ind_pos)
    {
        rl.clear();
        auto keys=mRows.keys();
        for (int i=0;i<keys.size();i++) {
            K k=keys.at(i);
            QList<tid> &vR=mRows[k];
            int r_i=q2findpos(ind_pos,vR);
            if (r_i>=0)
                rl.push_back(tsrezi(k,vR[r_i].data)); else
                rl.push_back(tsrezi(k,-1));
        }
        return rl;
    }

    void clear(){
        mRows.clear();
        lIndex.clear();
    }
    //int size() const {return vIndex.size;}
    //T_ind ind_val(int i) const {return vIndex[i];}
    int indexOf(T_ind index_val) const {
        bool ex=false;
        return q2find_ind_pos(index_val,ex);
    }
    QList<T_ind> lIndex;

protected:
    QMap<K,QList<tid>> mRows;
    QList<tsrezi> rl;

    int q2findpos(int npos, QList<tid> &pos)
    {
        // возвращаем равный или меньше
        int found_npos = -1;
        int l = 0; // левая граница
        int r = pos.size()-1; // правая граница
        int mid;
        if (pos.isEmpty()) return -1;
        if (pos.back().pos_in_index<npos) return pos.size()-1;
        while ((l <= r)) {
            mid = (l + r) / 2; // считываем срединный индекс отрезка [l,r]

            if (pos[mid].pos_in_index == npos) {found_npos=mid;break;} //проверяем ключ со серединным элементом
            if ((mid>0) && (pos[mid-1].pos_in_index<npos) && (pos[mid].pos_in_index>npos)) {found_npos=mid-1;break;}
            if (pos[mid].pos_in_index > npos) r = mid - 1; // проверяем, какую часть нужно отбросить
            else l = mid + 1;
        }
        return found_npos;
    }
    int q2find_ind_pos(T_ind ind_val,bool &ex) const
    {
        ex=false;
        // возвращаем равный или меньше
        int found_npos = -1;
        int l = 0; // левая граница
        int r = lIndex.size()-1; // правая граница
        int mid;
        if (lIndex.isEmpty()) return -1;
        if (lIndex.back()<ind_val) return lIndex.size()-1;
        while ((l <= r)) {
            mid = (l + r) / 2; // считываем срединный индекс отрезка [l,r]
            if (lIndex[mid] == ind_val) {found_npos=mid;ex=true;break;} //проверяем ключ со серединным элементом
            if ((mid>0) && (lIndex[mid-1]<ind_val) && (lIndex[mid]>ind_val)) {found_npos=mid-1;break;}
            if (lIndex[mid] > ind_val) r = mid - 1; // проверяем, какую часть нужно отбросить
            else l = mid + 1;
        }
        return found_npos;
    }

};

#endif // PARSEDVECTOR_H
