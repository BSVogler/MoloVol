#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#  include <wx/wx.h>
#endif

#include "base.h"
#include "special_chars.h"
#include <string>
#include <wx/msgdlg.h>

//////////////////////////////////
// METHODS FOR MANIPULATING GUI //
//////////////////////////////////

// THREAD SAFE

void MainFrame::extClearOutputText(){
  GetEventHandler()->CallAfter(&MainFrame::clearOutputText);
}

void MainFrame::extClearOutputGrid(){
  GetEventHandler()->CallAfter(&MainFrame::clearOutputGrid);
}

void MainFrame::extAppendOutput(const std::string text){
  GetEventHandler()->CallAfter(&MainFrame::appendOutput, text);
}

void MainFrame::extAppendOutputW(const std::wstring wtext){
  GetEventHandler()->CallAfter(&MainFrame::appendOutputW, wtext);
}

void MainFrame::extSetStatus(const std::string status){
  GetEventHandler()->CallAfter(&MainFrame::setStatus, status);
}

void MainFrame::extSetProgressBar(const int percentage){
  GetEventHandler()->CallAfter(&MainFrame::setProgressBar, percentage);
}

void MainFrame::extDisplayCavityList(const std::vector<Cavity>& cavities){
  GetEventHandler()->CallAfter(&MainFrame::displayCavityList, cavities);
}

void MainFrame::extOpenErrorDialog(const int error_code, const std::string& error_message){
  const std::pair<int, std::string> code_message = std::make_pair(error_code, error_message);
  GetEventHandler()->CallAfter(&MainFrame::openErrorDialog, code_message);
}

// NOT THREAD SAFE

void MainFrame::clearOutputText(){
  outputText->SetValue("");
}

void MainFrame::clearOutputGrid(){
  if (outputGrid->GetNumberRows() > 0){
    outputGrid->DeleteRows(0, outputGrid->GetNumberRows());
  }
}

void MainFrame::printToOutput(const std::string text){
  outputText->SetValue(text);
}

void MainFrame::appendOutput(const std::string text){
  outputText->SetValue(outputText->GetValue() + text);
}

void MainFrame::appendOutputW(const std::wstring text){
  outputText->SetValue(outputText->GetValue() + text);
}

std::string MainFrame::getAtomFilepath(){
  return filepathText->GetValue().ToStdString();
}

std::string MainFrame::getElementsFilepath(){
  return elementspathText->GetValue().ToStdString();
}

bool MainFrame::getIncludeHetatm(){
  return pdbHetatmCheckbox->GetValue();
}

bool MainFrame::getAnalyzeUnitCell(){
  return unitCellCheckbox->GetValue();
}

bool MainFrame::getCalcSurfaceAreas(){
  return surfaceAreaCheckbox->GetValue();
}

bool MainFrame::getProbeMode(){
  return twoProbesCheckbox->GetValue();
}

double MainFrame::getProbe1Radius(){
  return std::stod(probe1InputText->GetValue().ToStdString());
}

double MainFrame::getProbe2Radius(){
  return getProbeMode()? std::stod(probe2InputText->GetValue().ToStdString()) : 0;
}

double MainFrame::getGridsize(){
  return std::stod(gridsizeInputText->GetValue().ToStdString());
}

int MainFrame::getDepth(){
  return depthInput->GetValue();
}

bool MainFrame::getMakeReport(){
  return reportCheckbox->GetValue();
}

bool MainFrame::getMakeSurfaceMap(){
  return surfaceMapCheckbox->GetValue();
}

bool MainFrame::getMakeCavityMaps(){
  return cavityMapsCheckbox->GetValue();
}

std::string MainFrame::getOutputDir(){
  return dirpickerText->GetValue().ToStdString();
}

void MainFrame::displayAtomList(std::vector<std::tuple<std::string, int, double>> symbol_number_radius){
  // delete all rows
  // DeleteRows causes an error if there are no rows
  if (atomListGrid->GetNumberRows() > 0) {
    atomListGrid->DeleteRows(0, atomListGrid->GetNumberRows());
  }

  for (size_t row = 0; row < symbol_number_radius.size(); row++) {
    atomListGrid->AppendRows(1, true);
    // column 1 (symbol of atom)
    atomListGrid->SetCellValue(row, 1, std::get<0>(symbol_number_radius[row]));
    atomListGrid->SetReadOnly(row,1,true);
    // column 2 (number of atom)
    atomListGrid->SetCellValue(row, 2, std::to_string(std::get<1>(symbol_number_radius[row])));
    atomListGrid->SetReadOnly(row,2,true);
    // column 3 (radius of atom)
    atomListGrid->SetCellValue(row, 3, std::to_string(std::get<2>(symbol_number_radius[row])));
    // column 0 (include checkbox)
    // if no radius is found for the element, color cells to point it to the user
    if (std::wcstod(atomListGrid->GetCellValue(row, 3), NULL) == 0){
      atomListGrid->SetCellBackgroundColour(row, 1, _col_grey_cell);
      atomListGrid->SetCellBackgroundColour(row, 2, _col_grey_cell);
      atomListGrid->SetCellBackgroundColour(row, 3, _col_red_cell);
    }
    else { // if a radius is found, include by default the element
      atomListGrid->SetCellValue(row, 0, "1");
    }
    // refresh the grid to enforce the update of cell values and parameters
    // without this, it was sometimes observed that the last cell was not updated properly
    atomListGrid->ForceRefresh();
  }
}

void MainFrame::displayCavityList(const std::vector<Cavity>& cavities){
  // delete all rows
  clearOutputGrid();
  // add appropriate nuber of rows
  outputGrid->AppendRows(cavities.size());
  for (int row = 0; row < outputGrid->GetNumberRows(); ++row){
    outputGrid->SetCellValue(row, 0, std::to_string(row+1));
    outputGrid->SetCellValue(row, 1, std::to_string(cavities[row].getVolume()));
    if(getCalcSurfaceAreas()){
      outputGrid->SetCellValue(row, 2, std::to_string(cavities[row].getSurfCore()));
      outputGrid->SetCellValue(row, 3, std::to_string(cavities[row].getSurfShell()));
    }
    outputGrid->SetCellValue(row, 4, cavities[row].getPosition());
    // set all cells read only
    for (int col = 0; col < outputGrid->GetNumberCols(); ++col){
      outputGrid->SetReadOnly(row,col,true);
    }
  }
}

std::unordered_map<std::string, double> MainFrame::generateRadiusMap(){
  std::unordered_map<std::string, double> radius_map;
  for (int i = 0; i < atomListGrid->GetNumberRows(); i++){
    if (atomListGrid->GetCellValue(i,0) == "1"){
      std::string symbol = atomListGrid->GetCellValue(i,1).wxString::ToStdString();
      double radius;
      atomListGrid->GetCellValue(i,3).ToDouble(&radius);
      radius_map[symbol] = radius;
    }
  }
  return radius_map;
}

double MainFrame::getMaxRad(){
  double max_rad = 0;
  double radius = 0;
  for (int i = 0; i < atomListGrid->GetNumberRows(); i++){
    if (atomListGrid->GetCellValue(i,0) == "1"){
      atomListGrid->GetCellValue(i,3).ToDouble(&radius);
      if (radius > max_rad){
        max_rad = radius;
      }
    }
  }
  return max_rad;
}

std::vector<std::string> MainFrame::getIncludedElements(){
  std::vector<std::string> included_elements;
  for (int i = 0; i < atomListGrid->GetNumberRows(); i++){
    if (atomListGrid->GetCellValue(i,0) == "1"){
        included_elements.emplace_back(atomListGrid->GetCellValue(i,1).wxString::ToStdString());
    }
  }
  return included_elements;
}

void MainFrame::setStatus(const std::string str){
  statusBar->SetStatusText(str);
}

void MainFrame::setProgressBar(const int percentage){
  progressGauge->SetValue(percentage);
}

////////////////////
// DIALOG POP UPS //
////////////////////

void MainFrame::openErrorDialog(const std::pair<int,std::string>& code_message){
  const int error_code = code_message.first;
  const std::string error_message = code_message.second;
  wxMessageDialog error_dialog(this, error_message, "Error Code " + std::to_string(error_code), wxICON_ERROR | wxOK | wxCENTRE);
  error_dialog.ShowModal();
}
