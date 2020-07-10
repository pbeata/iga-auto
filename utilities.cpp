#include "utilities.h"


// NOTE: WE KEEP THE ABAQUS "1-BASED" NUMBERING SYSTEM
// FOR NODES AND ELEMENTS !!!


template <typename T>
void print(vector<T> &v)
{
    const int n = v.size();
    for (int i = 0; i < n; ++i)
    {
        cout << v[i] << "  ";
    }
    cout << endl;
}


template <typename ValueType>
int ReadLine(string tmp, vector <ValueType>& row) //Read a line of a file separated by comma
{
    //fprintf(stdout, "Test tmp, %s\n", tmp.c_str());
    row.clear();
    int pre_pos = 0;
    // int RowIndex = 0;
    //int Pre;
    //bool Pre_Bool = false;
    for (size_t i = 0; i < tmp.size(); i++)
    {
        string tmp_str;
        if (tmp[i] == ',')
        {
            stringstream r;
            tmp_str = tmp.substr(pre_pos, i - pre_pos);
            r << tmp_str;
            ValueType tt;
            r >> tt;

            row.push_back(tt);
            pre_pos = i + 1;
            //Pre = i + 1;
            //Pre_Bool = true;
            //printf("Test 1\n");
        }
        else if (i == tmp.size() - 1)
        {
            //if ( Pre_Bool == true && Pre == i + 1 ){
            //  break;
            //}

            stringstream r;
            tmp_str = tmp.substr(pre_pos, i - pre_pos);
            unsigned int Temp = 0;
            for (unsigned int mm = 0; mm < tmp_str.size(); mm++){
                if (tmp_str[mm] != ' '){
                    break;
                }
                else{
                    Temp++;
                }
            }

            if (Temp == tmp_str.size()){
                break;
            }

            r << tmp_str;
            ValueType tt;
            r >> tt;
            row.push_back(tt);
            break;
        }
    }
    return 1;
}

int no(char* param)
{
    if ( param != NULL && strlen(param) > 0 ) {
        return 0;
    }
    return 1;
}

char* StringToUpper(char* str)
{
    if(!no(str)) {
    unsigned int i=0;
    while(str[i]) {
        str[i]=toupper(str[i]);
        i++;
    }
    }
    return str;
}

int MatchKeyword(const char* str,const char* key2)
{  
  char key1[250];
  strcpy(key1,str);
  int ismatch=0;

  //fprintf(stdout, "Test str, %s\n", str);

  if(strcmp(str,","))
  {
      char *KEY=StringToUpper(strtok(key1,","));
      //fprintf(stdout, "Test key1, KEY and key2, %s, %s, %s\n", key1, KEY, key2);
      if (!no(KEY) && strcmp(KEY,key2) == 0)
        ismatch=1;
  }
  return ismatch;
}


int MatchString(const char* str,const char* key2) {
  char key1[250];
  strcpy(key1,str);
  char* sptr=strstr(StringToUpper(key1),key2);
  if ( sptr != NULL)
    return sptr-key1+1;
  return 0;
}

void DivideTerms(string tmp, vector<string>* Terms)
{
    vector<string> Terms2;

    Terms2.clear();

    int pre_pos = 0;
    //int RowIndex = 0;
    for (size_t i = 0; i < tmp.size(); i++)
    {
        string tmp_str;
        if (tmp[i] == ',')
        {
            //stringstream r;
            tmp_str = tmp.substr(pre_pos, i - pre_pos);
            //r << tmp_str;
            //int tt;
            //r >> tt;

            Terms2.push_back(tmp_str);

            //if (RowIndex == WidthofInputLines -1) break;
            //++RowIndex;

            pre_pos = i + 1;
        }
        else if ( tmp[i] == '\r' ){
            tmp_str = tmp.substr(pre_pos, i-pre_pos);
            Terms2.push_back(tmp_str);
            break;
        }
        else if ( i == tmp.size() - 1 )
        {
            //stringstream r;
            tmp_str = tmp.substr(pre_pos, i+1-pre_pos);
            //r << tmp_str;
            //double tt;
            //r >> tt;

            Terms2.push_back(tmp_str);

            break;
        }
    }

    //*******************************Get rid of the spaces*******************************
    for (size_t i=0; i<Terms2.size(); i++){
        string Temp = "";
        
        for (size_t j=0; j<Terms2[i].size(); j++){
            if ((Terms2[i])[j] != ' '){
                Temp = Temp + (Terms2[i])[j];
            }
        }
        (*Terms).push_back(Temp);
        //fprintf(stdout, "Test Temp, %s\n", Temp.c_str());
    }
    //***********************************************************************************

    //for (size_t i=0; i<(*Terms).size(); i++){
    //  if ( i <(*Terms).size() - 1){
    //      fprintf(stdout, "__%s__", (*Terms)[i].c_str());
    //  }
    //  else{
    //      fprintf(stdout, "__%s__\n", (*Terms)[i].c_str());
    //  }
    //}
}

inline 
void setVec(const std::vector<double> &inVec, vec3 &outVec)
{
    // only works if inVec is size 3
    if (inVec.size() == 3)
    {
        outVec.x = inVec[0];
        outVec.y = inVec[1];
        outVec.z = inVec[2];
    }
    else
    {
        cout << "vector size error: must be a vec3 size!" << endl;
    }
}

inline
void interpolateNode(
    const double &t,    // value between (0, 1)
    const vec3 &A,      // bottom node
    const vec3 &B,      // top node
    vec3 &C)            // interpolated node
{
    vec3 D;

    D.x = B.x - A.x;
    D.y = B.y - A.y;
    D.z = B.z - A.z;

    C.x = A.x + (t * D.x);
    C.y = A.y + (t * D.y);
    C.z = A.z + (t * D.z);
}





int ReadABAQUSInputs(
    string OUTDIR, 
    string JOBNAME, 
    map <int, vector <double> > &Nodes,
    unordered_map <int, vector <int> > &U_Elements)
{

    map <int, int> UType_NumNodes;
    unordered_set <int> U_Elements_Coh;
    vector<int> NodeList;
    vector<int> ElemList;
    double UEL_EE, UEL_nu;

    const int LPatch =  1000000;
    const int LNode =   1000000;

    int numSolids = 0;
    int numShells = 0;
    int allElems  = 0;

    //================================================================

    vector <double> row;
    vector <int> row2;
    vector<string> Terms;

    char FileName[256];

#ifdef LINUX
    sprintf(FileName, "%s/%s.inp", OUTDIR.c_str(), JOBNAME.c_str());
#endif
#ifdef WINDOWS
    sprintf(FileName, "%s\\\\%s.inp", OUTDIR.c_str(), JOBNAME.c_str());
#endif

    LOG(" ");
    cout << FileName << endl;

    ifstream infile(FileName, ios::binary);

    if (infile.fail())
    {
        cout << "Error in reading ABAQUS input file.\n" << endl;
        exit(-3);
    }

    while (true){
        string tmp;
        getline(infile, tmp);

        if (tmp[0] == '*' && tmp[1] != '*'){

            Terms.clear();
            //Divide the Line into Several Terms Seperated by Comma:
            DivideTerms(tmp, &Terms);

            // if (MatchString(Terms[0].c_str(), "*ENDSTEP") && !(Terms[0][0] == '*' && Terms[0][1] == '*')){
            //  break;
            // }
            if (MatchString(Terms[0].c_str(), "*ENDINSTANCE") && !(Terms[0][0] == '*' && Terms[0][1] == '*')){
                break;
            }           
            else{

                if (MatchString(Terms[0].c_str(), "*UELPROPERTY") && !(Terms[0][0] == '*' && Terms[0][1] == '*')){
                    while (true){
                        row.clear();
                        string tmp2;
                        int previous = infile.tellg();
                        getline(infile, tmp2);
                        if ((tmp2[0] == '*' && tmp2[1] != '*') || tmp2.find_first_not_of(' ') == std::string::npos){
                            infile.seekg(previous, ios::beg);
                            break;
                        }
                        else if ((tmp2[0] == '*' && tmp2[1] == '*')){
                            continue;
                        }
                        else{
                            ReadLine(tmp2, row);
                            vector<double> row_Temp(row.begin(), row.end());
                            UEL_EE = row_Temp[0];
                            UEL_nu = row_Temp[1];
                            cout << "Materials: " << UEL_EE << " and " << UEL_nu << endl;
                            break;
                        }
                    }
                }

                else if (MatchString(Terms[0].c_str(), "*USERELEMENT") && !(Terms[0][0] == '*' && Terms[0][1] == '*')){
                    int num_nodes_real;
                    int type_real;
                    int mmax = Terms.size();
                    for (auto m = 1; m < mmax; m++){
                        if (MatchString(Terms[m].c_str(), "NODES=")){
                            
                            string num_nodes = Terms[m].substr(6);

                            stringstream ss;
                            ss << num_nodes;
                            ss >> num_nodes_real;
                            
                            //cout << num_nodes_real << endl;
                            //int Pause6; cin >> Pause6;

                            break;
                        }
                    }
                    for (auto m = 1; m < mmax; m++){
                        if (MatchString(Terms[m].c_str(), "TYPE=U")){

                            string type = Terms[m].substr(6);

                            stringstream ss;

                            ss << type;
                            ss >> type_real;

                            //cout << type_real << endl;
                            //int Pause6; cin >> Pause6;

                            break;
                        }
                    }

                    LOG(type_real);
                    LOG(num_nodes_real);
                    UType_NumNodes[type_real] = num_nodes_real;

                }

                else if (MatchString(Terms[0].c_str(), "*NODE") && !(Terms[0][0] == '*' && Terms[0][1] == '*') && !MatchString(Terms[0].c_str(), "*NODEOUTPUT")
                    && !MatchString(Terms[0].c_str(), "*NODEFILE")
                    && !MatchString(Terms[0].c_str(), "*NODEPRINT")
                    && !MatchString(Terms[0].c_str(), "*NODERESPONSE")
                    ){

                    while ( true ){
                        row.clear();
                        string tmp2;
                        int previous = infile.tellg();
                        getline(infile, tmp2);
                        if ((tmp2[0] == '*' && tmp2[1] != '*') || tmp2.find_first_not_of(' ') == std::string::npos){
                            infile.seekg(previous, ios::beg);
                            break;
                        }
                        else if ((tmp2[0] == '*' && tmp2[1] == '*')){
                            continue;
                        }
                        else{
                            ReadLine(tmp2, row);
                            vector<double> row_Temp(row.begin() + 1, row.end());
                            
                            Nodes[ (int) row[0] ] = row_Temp;
                            NodeList.push_back((int) row[0]);

                            if ((int)row[0] > LNode){
                                cout << "Error: The number of nodes in the input file is larger than the one specified in the .iga file.\n";
                                exit(-3);
                            }

                            //cout << "Test 1: " << row[0] << ", " << row_Temp[0] << ", " << row_Temp[1] << ", " << row_Temp[2] << endl;

                        }
                    }
                }
                else if (MatchKeyword(Terms[0].c_str(), "*ELSET")){ // && MatchKeyword(Terms[1].c_str(), "ELSET=COHESIVE_SET") ){

                    //cout << "Test Terms.size() : " << Terms.size() << ", " << Terms[0] << ", " << Terms[1] << endl;

                    string dumm = Terms[1];
                    transform(dumm.begin(), dumm.end(), dumm.begin(), ::toupper);

                    //cout << "Test dumm: " << dumm << endl;
                    //setbuf(stdout,NULL);

                    if (dumm.compare("ELSET=COHESIVE_SET") == 0){

                        if (Terms.size() >= 3 && (MatchKeyword(Terms[2].c_str(), "GENERATE") || MatchKeyword(Terms[2].c_str(), "GEN"))){
                            string tmp2;
                            getline(infile, tmp2);
                            stringstream ss(tmp2);
                            string token;
                            vector <int> nums;
                            while (getline(ss, token, ',')){
                                nums.push_back(stoi(token));
                            }
                            for (auto m = nums[0]; m <= nums[1]; m += nums[2]){
                                U_Elements_Coh.insert(m);
                            }

                            //set <int> dummy(U_Elements_Coh.begin(), U_Elements_Coh.end());
                            //for (auto iter = dummy.begin(); iter != dummy.end(); iter++){
                            //  cout << "Test U_Elements_Coh1: " << *iter << endl;
                            //}
                        }
                        else{
                            while (true){
                                int position = infile.tellg();
                                string tmp2;
                                getline(infile, tmp2);
                                stringstream ss(tmp2);
                                if ((tmp2.size() >= 2 && tmp2[0] == '*' && tmp2[1] != '*') || tmp2.find_first_not_of(' ') == std::string::npos){
                                    infile.seekg(position, ios::beg);
                                    break;
                                }
                                else if ((tmp2.size() >= 2 && tmp2[0] == '*' && tmp2[1] == '*')){
                                    continue;
                                }
                                else{
                                    string token;
                                    while (getline(ss, token, ',')){
                                        try{
                                            U_Elements_Coh.insert(stoi(token));
                                        }
                                        catch (...){
                                            break;
                                        }
                                    }
                                }

                            }
                            
                            //set <int> dummy(U_Elements_Coh.begin(), U_Elements_Coh.end());
                            //for (auto iter = dummy.begin(); iter != dummy.end(); iter++){
                            //  cout << "Test U_Elements_Coh1: " << *iter << endl;
                            //}
                        }
                    }

                }
                else if (
                            MatchKeyword(Terms[0].c_str(), "*ELEMENT") && ( MatchString(Terms[1].c_str(), "TYPE=U") || MatchString(Terms[2].c_str(), "TYPE=U") )
                        )
                { //Read Information of the User Defined Elements

                    //cout << "TEst Cohesive_Flag: " << Cohesive_Flag << endl;
                    //cout << "Terms.size(): " << Terms.size() << endl;

                    //string Cohesive_Set;
                    //bool Coh_flag = false;
                    //if (Terms.size() >= 3 && Cohesive_Flag){
                    //  Cohesive_Set = Terms[2].substr(6, 18);
                    //  transform(Cohesive_Set.begin(), Cohesive_Set.end(), Cohesive_Set.begin(), ::toupper);
                    //  if (Cohesive_Set == "COHESIVE_SET"){
                    //      //cout << "Found the set!\n";
                    //      Coh_flag = true;
                    //  }
                    //}             

                    string utype = Terms[1].substr(6);

                    stringstream ss;

                    int utype_num;
                    ss << utype;
                    ss >> utype_num;
                    int tmp = 0;
                    int numnodes = UType_NumNodes[utype_num];
                    //cout << numnodes << endl;

                    int Ele_Index;
                    while (true){
                        row2.clear();
                        string tmp2;
                        int previous = infile.tellg();
                        getline(infile, tmp2);
                        if ((tmp2[0] == '*' && tmp2[1] != '*') || tmp2.find_first_not_of(' ') == std::string::npos){
                            infile.seekg(previous, ios::beg);
                            break;
                        }
                        else if ((tmp2[0] == '*' && tmp2[1] == '*')){
                            continue;
                        }
                        else{
                            ReadLine(tmp2, row2);
                            
                            //U_Connectivities.push_back(row2);
                            
                            if (tmp == 0){
                                vector<int> row2_Temp(row2.begin() + 1, row2.end());
                                U_Elements[row2[0]] = row2_Temp;
                                Ele_Index = row2[0];
                                tmp += row2_Temp.size();

                                allElems++;

                                // print(row2_Temp);
                                // if (utype_num >= Ua && utype_num <= Ub) numSolids++;
                                // else numShells++;    

                                //if (Coh_flag){
                                //  //cout << "Found the one.\n";
                                //  U_Elements_Coh.insert(Ele_Index);
                                //}

                                if (Ele_Index > LPatch){
                                    cout << "Error: The number of elements in the input file is larger than the one specified in the .iga file.\n";
                                    exit(-3);
                                }

                            }
                            else{
                                vector<int> row2_Temp(row2.begin(), row2.end());
                                U_Elements[Ele_Index].insert(U_Elements[Ele_Index].end(), row2_Temp.begin(), row2_Temp.end());
                                tmp += row2_Temp.size();

                                // print(row2_Temp);
                            }

                            if (tmp == numnodes){
                                tmp = 0;

                                // check:
                                // vector<int> ien = U_Elements[Ele_Index];
                                // print(ien);

                                ElemList.push_back(Ele_Index);

                                //for (auto nn = 0; nn < (*U_Elements)[Ele_Index].size(); nn++){
                                //  printf("Test ele: %d, %d\n", Ele_Index, (*U_Elements)[Ele_Index][nn]);
                                //}
                                //int Pause12; cin>>Pause12;
                            }
                            else if (tmp > numnodes){
                                cout << "Error! " << tmp << " and " << numnodes << "\n" << endl;
                                exit(-3);
                            }

                            //for (auto mmm = 0; mmm < row2_Temp.size(); mmm++){
                            //  cout << row2_Temp[mmm] << endl;
                            //}
                            //int Pause; cin >> Pause;

                            //cout << "Test 1: " << row2[0] << ", " << row2_Temp[0] << ", " << row2_Temp[1] << ", " << row2_Temp[2] << endl;

                        }
                    }
                }

            }
        }
        //***************************************************************************************************
    }

    //U_Elements
    // for (auto iter = (*U_Elements).begin(); iter != (*U_Elements).end(); iter++){
    //  int ele_index = iter->first;
    //  int nnode = iter->second.size();

        // delete
        // U_Elements_pU[ele_index] = new double[nnode * 3];
        // if (Cohesive_Flag)
            // U_Elements_coh_state[ele_index] = new double[6];
        // delete

    // }

    cout << "number of solids = " << numSolids << endl;
    cout << "number of shells = " << numShells << endl;
    cout << "total # of elems = " << allElems << endl;

    // save to IGA SOLID 
    // Num_Solids = numSolids;
    // Num_Elems_Total = allElems;


//===================================================================




    // cout << "\nCHECK NODES: " << endl;
    const int NNODE = Nodes.size();
    // const int DIM = 3;
    int id;

    for (int i = 0; i < NNODE; ++i) {

        id = NodeList[i];
        vector<double> node = Nodes[id];
        // cout << id << endl;

        // cout << id;
        // for (int j = 0; j < DIM; ++j) {
        //  cout << ", " << node[j];
        // }
        // cout << endl;
    }



    // cout << "\nCHECK CONNECTIVITY: " << endl;
    const int NELEM = ElemList.size();

    for (int i = 0; i < NELEM; ++i)
    {
        id = ElemList[i];
        // vector<int> ien = U_Elements[id];
        // print(ien);

        // cout << id << endl;      
    }


    // for (auto iter = U_Elements.begin(); iter != U_Elements.end(); iter++)
    // {
    //     int ele_index = iter->first;
    //     std::cout << ele_index << ", " << U_Elements[ele_index].size() << std::endl;
    // } 


    return 1;
}




void ReadTSplineData(
    const char* filename, vector<double>& t, 
    int& nlayers, vector<double>& fiber_angle, 
    int& nsections, vector<int>& section_limits, 
    vector<double>& knotW, vector<double>& elRangeW, 
    vector<double>& coords, vector<double>& weight, 
    unordered_map<int, vector<int>>& IEN, unordered_map<int,vector<vector<double>>>& BEXT,
    const int& nsolids, const int& poly3) 
{
    cout << "\n**Start reading t-spline input file.\n";
    string tmp2;
    vector<string> Terms;
    vector<int> row;
    vector<double> row2;
    vector<vector<double>> mat2;
    int ielem = 0;

    //===============================================================
    // const bool merged_UEL = true;
    // if (merged_UEL)
    // {
    //     cout << "**WARNING: changed the ReadTSplineData function";
    //     cout << " to account for the new merged UEL analysis: ";
        
    //     // start the map keys for IEN and BEXT at the proper number
    //     ielem = nsolids;
    //     cout << ielem << " (non-zero ielem)\n";
    // }
    //===============================================================

    ifstream infile(filename);
    if (infile.fail()) {
        cout << "Error in opening the job.tsplines file.\n" << endl;
        system("pause");
        exit(-3);
    }
    string tmp;
    while (getline(infile, tmp)) {
        if (tmp[0] == '*' && tmp[1] != '*') {
            Terms.clear();
            DivideTerms(tmp, &Terms);
            if (MatchString(Terms[0].c_str(), "COORDINATES")) {
                cout << "Start reading coordinate information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        stringstream ss(tmp2);
                        double tmp3;
                        while (ss >> tmp3) {
                            coords.push_back(tmp3);
                        }
                    }
                }
                cout << "  End reading coordinate information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "THICKNESS")) {
                cout << "Start reading layer, thickness, and fiber angle information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        if (row2.size() == 1)
                            t.push_back(row2[0]);
                        else {
                            cout << "Thickness input format error (should be one layer thickness per line).\n" << endl;
                            system("pause");
                            exit(-4);
                        }
                    }
                }
                // cout << "  End reading thickness information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "NUMBEROFLAYERS")) {
                while (true) {
                    row.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row);
                        if (row.size() == 1) {
                            nlayers = row[0];
                        }
                        else {
                            cout << "NoLayers input format error (should be a scalar).\n" << endl;
                            system("pause");
                            exit(-4);
                        }
                    }
                }
            }

            else if (MatchString(Terms[0].c_str(), "FIBERANGLE")) {
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        if (row2.size() == 1)
                            fiber_angle.push_back(row2[0]);
                        else {
                            cout << "Fiber angle input format error (should be one layer angle per line).\n" << endl;
                            system("pause");
                            exit(-4);
                        }
                    }
                }
                cout << "  End reading #layer and fiber angle information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "NUMBEROFSECTIONS")) {
                cout << "Start reading shell section information.\n";
                while (true) {
                    row.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row);
                        if (row.size() == 1) {
                            nsections = row[0];
                        }
                        else {
                            cout << "Number of sections input format error (should be a scalar).\n" << endl;
                            system("pause");
                            exit(-4);
                        }
                    }
                }
            }

            else if (MatchString(Terms[0].c_str(), "SECTIONLIMITS")) {
                // cout << "Start reading shell section information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        for (unsigned int i = 0; i < row2.size(); i++) {
                            section_limits.push_back(row2[i]);
                        }
                    }
                }
                cout << "  End reading shell section information.\n";
            }           

            else if (MatchString(Terms[0].c_str(), "COORDINATES")) {
                cout << "Start reading control point coordinate information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        if (row2.size() == 3) {
                            for (int i = 0; i < 3; i++)
                                coords.push_back(row2[i]);
                        }
                        else {
                            cout << "Coordinates input format error (dimension should be 3).\n" << endl;
                            system("pause");
                            exit(-4);
                        }
                    }
                }
                cout << "  End reading control point coordinate information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "WEIGHTS")) {
                cout << "Start reading control point weight information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        if (row2.size() == 1) {
                            weight.push_back(row2[0]);
                        }
                        else {
                            cout << "Control point weight input format error.\n" << endl;
                            system("pause");
                            exit(-4);
                        }
                    }
                }
                cout << "  End reading control point weight information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "KNOTW")) {
                cout << "Start reading knot vector information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        for (unsigned int i = 0; i < row2.size(); i++) {
                            knotW.push_back(row2[i]);
                        }
                    }
                }
                cout << "  End reading knot vector information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "ELRANGEW")) {
                cout << "Start reading element range information.\n";
                while (true) {
                    row2.clear();
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        ReadLine(tmp2, row2);
                        for (unsigned int i = 0; i < row2.size(); i++) {
                            elRangeW.push_back(row2[i]);
                        }
                    }
                }
                cout << "  End reading element range information.\n";
            }

            else if (MatchString(Terms[0].c_str(), "BEXT")) {
                cout << "Start reading bezier information.\n";
                while (true){
                    string tmp2;
                    int prev = infile.tellg();
                    getline(infile, tmp2);
                    if (tmp2[0] == '*' || tmp2.find_first_not_of(' ') == string::npos) {
                        infile.seekg(prev, ios::beg);
                        break;
                    }
                    else {
                        stringstream ss(tmp2);
                        int icount = 0;
                        int nrows;
                        string tmp3;
                        while (ss >> tmp3) {
                            if (icount == 1) {
                                nrows = stoi(tmp3.c_str());
                                nrows = nrows / (poly3 + 1);
                                break;
                            }
                            icount++;
                        }
                        //read in IEN information
                        getline(infile, tmp3);
                        stringstream ss2(tmp3);
                        row.clear();
                        int inode;
                        while (ss2 >> inode) {
                            row.push_back(inode);
                        }
                        IEN[ielem] = row;

                        //read in Bezier extraction matrix of current element
                        mat2.resize(nrows);
                        for (int irow = 0; irow < nrows; irow++) {
                            getline(infile, tmp3);
                            stringstream ss(tmp3);
                            row2.clear();
                            double bext_terms;
                            while (ss >> bext_terms) {
                                row2.push_back(bext_terms);
                            }
                            mat2[irow] = row2;
                        }

                        BEXT[ielem] = mat2;
                        ielem++;            
                    }
                }
                cout << "  End reading bezier information.\n";
            }
        }
    }
    infile.close();

    // cout << "\nCHECK SECTION INPUT \n";
    // cout << "number of sections: " << nsections << endl;
    // const int nlimits = section_limits.size();
    // for (int i = 0; i < nlimits; ++i)
    // {
    //  cout << section_limits[i] << "\t";
    // }
    // cout << "\n\n";

    cout << "**End reading t-spline input file.\n";
}






void generateThruThicknessData(
    const int &NL, 
    const int &poly3,
    const std::map <int, vector <double> > &NODES_bot, 
    const std::unordered_map <int, vector <int> > &IEN_newbot,
    const std::map <int, vector <double> > &NODES_top, 
    const std::unordered_map <int, vector <int> > &IEN_top,
    std::unordered_map <int, vector <double> > &NODES, 
    std::unordered_map <int, user_element> &MESH)
{

    // number of planes of control points through the thickness
    const int numPlanes = NL * poly3 + 1;
    const double dt = 1.0 / (numPlanes - 1);

    unordered_map < int, vector<int> > IEN;

    // add all the nodes on the bottom to the NODES structure
    //  (we assume the bottom surface nodes are labeled 1 to N)
    
    int id;
    double t;
    // int count = 0;


    const int m = NODES_bot.size();
    const int n = NODES_top.size();

    vec3 botNode;
    vec3 topNode;
    vec3 intNode;
    vector<int> ien;
    vector<double> temp;

    if (m != n) {
        cout << "ERROR: number of nodes in the top set does not match bottom set" << endl;
    }
    else {

        for (auto const& it : NODES_bot) {
            // std::cout << it.first << std::endl;
            id = it.first;
            NODES[id] = it.second;
            
            // define the top and bottom nodes
            setVec(it.second, botNode);
            temp = NODES_top.at(id + m);
            setVec(temp, topNode);

            // generate nodes between top and bottom surfaces and add them to NODES
            t = dt;
            for (int k = 1; k < (numPlanes - 1); ++k) {
                interpolateNode(t, botNode, topNode, intNode);

                temp[0] = intNode.x;
                temp[1] = intNode.y;
                temp[2] = intNode.z;
                NODES[id + k * n] = temp;

                t += dt;
            }
        }
    }


    // add all the nodes on the top to the NODES structure
    int count = NODES.size();
    for (auto const& it : NODES_top)
    {
        // std::cout << it.first << std::endl ;
        id = it.first;
        count++;
        NODES[count] = it.second;
    }

    // write all nodes to new .inp file
    string finp = "test.inp";
    std::ofstream outFile(finp);
    outFile.precision(8);
    outFile.setf( std::ios::fixed, std:: ios::floatfield );  // floatfield set to fixed   
    
    if (outFile.is_open()) {

        outFile << "*HEADING" << endl;
        outFile << "*PART, NAME=PART-1" << endl;

        const int N = NODES.size();
        outFile << "*NODE" << endl;
        for (int i = 1; i <= N; ++i) {           
            outFile << i;
            temp = NODES.at(i);
            for (int j = 0; j < DIM; ++j) {
                outFile << ", " << temp[j];
            }
            outFile << "\n";
        }        
        //=================================================


        vector<int> elem;
        unordered_map< int, vector<int> > elem_type;
        int nnode;
        // user_element test;
        // const int numElemPlane = IEN_newbot.size();

        // for (int k = 0; k < NL; ++k) {
        for (auto const& it : IEN_newbot) {
            
            id = it.first;
            // id -= Ntop;

            elem = it.second;
            nnode = elem.size();
            // cout << id << "  " << nnode << endl;
            elem_type[nnode].push_back(id);

            user_element test;
            test.unodes = nnode * (poly3 + 1);
            test.utype = nnode;
            test.uproperties = PROPS;
            test.ucoordinates = DIM;
            test.uvariables = SVARS;

            MESH[nnode] = test;
        }
        // }

        vector <int> ien, ien_full;
        int k, nelem, inode;
        const int max_col = 16;
        const int NELEM = IEN_newbot.size();

        for (auto const& it : elem_type) {
            
            user_element test = MESH.at(it.first);
            outFile << "**\n*USER ELEMENT";
            outFile << ", NODES=" << test.unodes;
            outFile << ", TYPE=U" << test.utype;
            outFile << ", PROP=" << test.uproperties;
            outFile << ", COORD=" << test.ucoordinates;
            outFile << ", VAR=" << test.uvariables;
            outFile << "\n1, 2, 3" << endl;

            outFile << "*ELEMENT";
            outFile << ", TYPE=U" << test.utype;
            outFile << ", ELSET=UEL" << test.utype;
            outFile << endl;

            nelem = it.second.size();
            for (int i = 0; i < nelem; ++i) {
                
                id = it.second[i];
                ien.clear();
                ien = IEN_newbot.at(id);
                nnode = ien.size();

                for (int ilayer = 0; ilayer < NL; ++ilayer) {
                    // update element index
                    id += (ilayer * NELEM);
                    outFile << id;
                    k = 1;

                    // update the ien to start at the bottom of the layer
                    for (int j = 0; j < nnode; ++j) {
                        ien[j] += ilayer * m * poly3;
                    }

                    // loop over the control point layers in the element
                    for (int l = 0; l <= poly3; ++l) {
                        for (int j = 0; j < nnode; ++j) {

                            // update node through thickness
                            inode = ien[j] + (l * m);
                            ien_full.push_back(inode);

                            // write to output file
                            if (k < max_col) {
                                outFile << ", " << inode;
                                k++;
                            }
                            else {
                                outFile << ",\n" << inode;
                                k = 1;
                            }
                        }
                    }
                    outFile << endl;
                    IEN[id] = ien_full;
                }
            }
        }

        outFile << "**\n*ELSET, ELSET=ALL_UEL, GENERATE" << endl;
        outFile << "1, " << IEN.size() << ", 1" << endl;

        outFile << "*UEL PROPERTY, ELSET=ALL_UEL" << endl;
        outFile << "6.825e7, 6.825e7, 6.825e7, 2.625e7, 2.625e7, 2.625e7, 0.3, 0.3," << endl;
        outFile << "0.3" << endl;

        outFile << "*END PART\n**" << endl;
        outFile << "**==========================================================" << endl;


        


        outFile.close();
    }
    else {
        LOG("you forgot to open the file!");
    }

    LOG("finished writing to output file")
}

