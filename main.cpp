#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <ctype.h>
#include <unordered_map>

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
        if (isdigit(current_char)){
            fields.push_back(current_char - '0');
        }
    }
    in_file.close();
}

set<int> determine_certain_value_variant_one(set<int> &horizontal_set, set<int> &vertical_set, set<int> &cluster_set){
    //First Intersetion
    set<int> result_step_one;
    set_intersection(horizontal_set.begin(), horizontal_set.end(), vertical_set.begin(), vertical_set.end(), inserter(result_step_one, result_step_one.begin()));
    //Second Intersection with both of first Intersection
    set<int> final_result;
    set_intersection(result_step_one.begin(), result_step_one.end(), cluster_set.begin(), cluster_set.end(), inserter(final_result, final_result.begin()));
    return final_result;
}

set<int> get_all_horizontal_indices(int index, int n, vector<int> &fields){
    set<int> all_horizontal_indices;
    for (int i = 0; i < n; ++i) {
        int pos_index = get_horizontal(index, n) * n + i;
        if (fields.at(pos_index) == 0 || true) {
            all_horizontal_indices.insert(pos_index);
        }
    }
    all_horizontal_indices.erase(index);
    return all_horizontal_indices;
}

set<int> get_all_vertical_indices( int index, int n, vector<int> &fields){
    set<int> all_vertical_indices;
    for (int i = 0; i < n; ++i) {
        int pos_index = i * n + get_vertical(index, n);
        if (fields.at(pos_index) == 0 || true) {
            all_vertical_indices.insert(pos_index);
        }
    }
    all_vertical_indices.erase(index);
    return all_vertical_indices;
}

set<int> get_all_cluster_indices(int index, int root_n, vector<int> &fields){
    set<int> all_cluster_indices;
    int start_x = get_cluster(index, root_n)%root_n * root_n;
    int start_y = get_cluster(index, root_n)/root_n * root_n;
    for (int x = start_x; x < start_x + root_n; ++x) {
        for (int y = start_y; y < start_y + root_n; ++y) {
            int pos_index = y * root_n * root_n + x;
            if (fields.at(pos_index) == 0 || true) {
                all_cluster_indices.insert(pos_index);
            }
        }
    }
    all_cluster_indices.erase(index);
    return all_cluster_indices;
}

bool is_value_valid_here(int value, set<int> &horizontal_set, set<int> &vertical_set, set<int> &cluster_set){
    return (determine_certain_value_variant_one(horizontal_set, vertical_set, cluster_set).count(value)) != 0;
}

bool is_value_valid_somewhere_else(set<int> &other_indices, int value, vector<set<int>> &horizontal_dimensions, vector<set<int>> &vertical_dimensions, vector<set<int>> &cluster_dimensions, int n, int root_n, vector<int> &fields){
    //checken, ob Wert schon irgendwo in der Dimension platziert wurde
    set<int>::iterator iterator_other_indices = other_indices.begin();
    while (iterator_other_indices != other_indices.end()){
        int index = *iterator_other_indices;
        if(fields.at(index) == value || (fields.at(index) == 0 && is_value_valid_here(value, horizontal_dimensions.at(get_horizontal(index, n)), vertical_dimensions.at(get_vertical(index, n)), cluster_dimensions.at(get_cluster(index, root_n))))){
            return true;
        }
        iterator_other_indices++;
    }
    return false;
}

int determine_certain_value_variant_two(set<int> &intersection_result, vector<set<int>> &horizontal_dimensions, vector<set<int>> &vertical_dimensions, vector<set<int>> &cluster_dimensions, int index, int root_n, vector<int> &fields){
    int n = root_n * root_n;

    set<int>::iterator iterator_pos_value = intersection_result.begin();

    while (iterator_pos_value != intersection_result.end()){
        int value = *iterator_pos_value;
        set<int> horizontal_indices = get_all_horizontal_indices(index, n, fields);
        set<int> vertical_indices = get_all_vertical_indices(index, n, fields);
        set<int> cluster_indices = get_all_cluster_indices(index, root_n, fields);
        if (!is_value_valid_somewhere_else(horizontal_indices, value, horizontal_dimensions, vertical_dimensions, cluster_dimensions, n, root_n, fields)
        || !is_value_valid_somewhere_else(vertical_indices, value, horizontal_dimensions, vertical_dimensions, cluster_dimensions, n, root_n, fields)
        || !is_value_valid_somewhere_else(cluster_indices, value, horizontal_dimensions, vertical_dimensions, cluster_dimensions, n, root_n, fields)
        ) {
            return value;
        }
        iterator_pos_value++;
    }
    return 0;
}

int determine_certain_value(set<int> &horizontal_set, set<int> &vertical_set, set<int> &cluster_set, vector<set<int>> &horizontal_dimensions, vector<set<int>> &vertical_dimensions, vector<set<int>> &cluster_dimensions, vector<int> &fields, int index, int root_n){
    set<int> final_result = determine_certain_value_variant_one(horizontal_set, vertical_set, cluster_set);
    if(final_result.size() == 1){
        //1. Variante funktioniert
        return *(final_result.begin());
    } else{
        //1. Variante funktioniert nicht -> 2. Variante prüfen
        return determine_certain_value_variant_two(final_result, horizontal_dimensions, vertical_dimensions, cluster_dimensions, index, root_n, fields);
        //return 0;
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

void fill_dimensions(vector<set<int>> &horizontal_dimensions, vector<set<int>> &vertical_dimensions, vector<set<int>> &cluster_dimensions, int n){
    set<int> full_set;
    fill_full_set(full_set, n);
    for (int i = 0; i < n; ++i) {
        horizontal_dimensions.push_back(full_set);
        vertical_dimensions.push_back(full_set);
        cluster_dimensions.push_back(full_set);
    }
}

int main() {

    //Felder
    vector<int> fields;

    string filename;
    cout << "Filename:\n";
    cin >> filename;

    read_file("../beispiele/"+filename, fields);

    int root_n = 3;
    int n = root_n * root_n;

    //Dimensionen
    vector<set<int>> horizontal_dimensions;
    vector<set<int>> vertical_dimensions;
    vector<set<int>> cluster_dimensions;
    fill_dimensions(horizontal_dimensions, vertical_dimensions, cluster_dimensions, n);

    queue<int> affected_queue;
    set<int> affected_set;
    int horizontal, vertical, cluster;

    //Das erste Mal alle offenen Felder der Schlange hinzufügen
    for (int j = 0; j < fields.size(); ++j) {
        if (fields.at(j)){ //größer als 0, vorher festgelegtes Feld
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
        int determine_result = determine_certain_value(horizontal_dimensions.at(horizontal), vertical_dimensions.at(vertical), cluster_dimensions.at(cluster), horizontal_dimensions, vertical_dimensions, cluster_dimensions, fields, index, root_n);

        if (determine_result > 0){ //oder eine andere Variante ist erfüllt, bisher nur Variante 1 betrachtet -> todo
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

    cout << "Result:";
    for (int k = 0; k < fields.size(); ++k) {
        if (k % n == 0){
            cout << endl;
        }
        cout << fields.at(k) << " ";
    }

    return 0;
}