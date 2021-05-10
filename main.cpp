#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <ctype.h>

using namespace std;

inline int get_horizontal(int index, int n){
    return index/n;
}

inline int get_vertical(int index, int n){
    return index%n;
}

inline int get_cluster(int index, int root_n){
    int n = root_n * root_n;
    return index/(n * root_n) * root_n + (index%n)/root_n;
}

void read_file(string filename, vector<int> &fields){
    ifstream in_file(filename, ios::in);

    if(!in_file) {
        cout << "Error while reading files" << endl;
    }

    char current_char;
    while (in_file.get(current_char)){
        cout << current_char << " " << isdigit(current_char) << endl;
        if (isdigit(current_char)){
            fields.push_back(current_char - '0');
        }
    }
    in_file.close();
}

int determine_certain_value(set<int> &horizontal_set, set<int> &vertical_set, set<int> &cluster_set){
    //First Intersetion
    set<int> result_step_one;
    set_intersection(horizontal_set.begin(), horizontal_set.end(), vertical_set.begin(), vertical_set.end(), inserter(result_step_one, result_step_one.begin()));
    //Second Intersection with both of first Intersection
    set<int> final_result;
    set_intersection(result_step_one.begin(), result_step_one.end(), cluster_set.begin(), cluster_set.end(), inserter(final_result, final_result.begin()));

    if(final_result.size() == 1){
        return *(final_result.begin());
    } else{
        return 0;
    }
}

void append_affected_fields_to_queue(int index_changed_field, vector<int> &fields, queue<int> &affected_queue, set<int> &affected_set, int n, int root_n, int cluster){
    set<int> affected_fields_indices;

    for (int i = 0; i < n; ++i) {
        affected_fields_indices.insert(n * (index_changed_field/n) + i); //horizontal dimension
        affected_fields_indices.insert(i * n + index_changed_field % n); //vertical dimension
    }

    int start_x = cluster%root_n * root_n;
    int start_y = cluster/root_n * root_n;
    for (int x = start_x; x < start_x + root_n; ++x) {
        for (int y = start_y; y < start_y + root_n; ++y) {
            affected_fields_indices.insert(y * n + x);
        }
    }

    affected_fields_indices.erase(index_changed_field);

    set<int>::iterator it = affected_fields_indices.begin();
    while (it != affected_fields_indices.end()){
        if (affected_set.count(*it) == 0 && !fields.at(*it)) {
            //nur einfügen, falls das Element noch nicht in der Warteschlange ist
            affected_queue.push(*it);
            affected_set.insert(*it);
        }
        it++;
    }

}

void fill_full_set(set<int> &sample_set, int n){
    for (int i = 1; i <= n; ++i) {
        sample_set.insert(i);
    }
}

int main() {

    //Felder
    vector<int> fields;

    read_file("../beispiele/1.txt", fields);

    int root_n = 3;
    int n = root_n * root_n;

    //Dimensionen
    vector<set<int>> horizontal_dimensions;
    vector<set<int>> vertical_dimensions;
    vector<set<int>> cluster_dimensions;

    set<int> full_set;
    fill_full_set(full_set, n);

    for (int i = 0; i < n; ++i) {
        horizontal_dimensions.push_back(full_set);
        vertical_dimensions.push_back(full_set);
        cluster_dimensions.push_back(full_set);
    }

    queue<int> affected_queue;
    set<int> affected_set;
    int horizontal, vertical, cluster;

    //Das erste Mal alle offenen Felder der Schlange hinzufügen
    for (int j = 0; j < fields.size(); ++j) {
        if (fields.at(j)){ //größer als 0
            int value = fields.at(j);
            horizontal = get_horizontal(j, n);
            horizontal_dimensions.at(horizontal).erase(value);
            vertical = get_vertical(j, n);
            vertical_dimensions.at(vertical).erase(value);
            cluster = get_cluster(j, root_n);
            cluster_dimensions.at(cluster).erase(value);
        } else{
            //offenes Feld
            affected_queue.push(j);
            affected_set.insert(j);
        }
    }

    while (!affected_queue.empty()){
        int index = affected_queue.front();
        affected_queue.pop();
        affected_set.erase(index);

        horizontal = get_horizontal(index, n);
        vertical = get_vertical(index, n);
        cluster = get_cluster(index, root_n);
        int determine_result = determine_certain_value(horizontal_dimensions.at(horizontal), vertical_dimensions.at(vertical), cluster_dimensions.at(cluster));

        if (determine_result > 0){
            //Ein Wert ist eindeutig zuordenbar
            //Wert speichern
            fields.at(index) = determine_result;
            //Die verwendete Zahl als  Möglichkeit für die betroffenen drei Dimensionen löschen
            horizontal_dimensions.at(horizontal).erase(determine_result);
            vertical_dimensions.at(vertical).erase(determine_result);
            cluster_dimensions.at(cluster).erase(determine_result);

            //Weitere Felder der Warteschlange hinzufügen
            append_affected_fields_to_queue(index, fields, affected_queue, affected_set, n, root_n, cluster);
        }
    }

    for (int k = 0; k < fields.size(); ++k) {
        if (k % n == 0){
            cout << endl;
        }
        cout << fields.at(k) << " ";
    }

    return 0;
}