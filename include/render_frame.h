#ifndef RENDER_FRAME_H

#define RENDER_FRAME_H

// Custom library
#include "wxVTKRenderWindowInteractor.h"

// wxWidgets
#include <wx/wx.h>

// VTK
#include <vtkSmartPointer.h>

// Standard library
#include <stdlib.h>

class vtkNamedColors;
class vtkImageData;
class vtkImageMask;
class vtkMarchingCubes;
class vtkRenderer;
class vtkRenderWindow;
class vtkPolyDataMapper;
class vtkActor;
class vtkMolecule;
class vtkMoleculeMapper;

class wxPanel;
class wxTextCtrl;
class wxStaticText;
class wxCommandEvent;
class wxButton;
class wxListCtrl;

template <typename> class Container3D;
class Voxel;
class MainFrame;

class RenderFrame : public wxFrame {
  public:
    RenderFrame(const MainFrame*, const wxString&, const wxPoint&, const wxSize&);
    ~RenderFrame();

    void OnClose(wxCloseEvent& event);
    void UpdateSurface(const Container3D<Voxel>&, const bool, const unsigned char);
    void Render();
  
    vtkSmartPointer<vtkNamedColors> colors;
    vtkSmartPointer<vtkImageData> imagedata;
    vtkSmartPointer<vtkImageData> maskdata;
    vtkSmartPointer<vtkImageMask> imagemask;
    vtkSmartPointer<vtkMarchingCubes> surface;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkMolecule> molecule;
    vtkSmartPointer<vtkMoleculeMapper> molmapper;
    vtkSmartPointer<vtkActor> molactor;
  
  private:
    void AdjustControls(bool);
    void ChangeIso(double);
    void ClearMask();

    // Events
    void OnChangeIso(wxCommandEvent&);
    void OnButtonClick(wxCommandEvent&);
    void OnCavitySelect(wxCommandEvent&);

    // Init
    void InitIsoInputPanel();
    void InitIsoControlPanel();
    void InitControlPanel();
    void InitPointerMembers();
    void InitRenderWindow();

    // Members
    bool m_twoProbeMode;
    const MainFrame* m_parentWindow;
  
    wxVTKRenderWindowInteractor* m_pVTKWindow;
    wxPanel* m_controlPanel;
      wxPanel* m_isoCtrlPanel;
        wxButton* m_vdwBtn;
        wxButton* m_molBtn;
        wxButton* m_cavityBtn;
        wxButton* m_accessibleBtn;
        wxPanel* m_isoInputPanel;
          wxStaticText* m_isoText;
          wxTextCtrl* m_isoCtrl;
      wxButton* m_resetCameraBtn;
      wxListBox* m_cavityList;
  
    DECLARE_EVENT_TABLE()
};

enum {
  WXVTK_Render = wxID_HIGHEST+1,
  PANEL_Control,
  PANEL_IsoCtrl,
  BUTTON_Vdw,
  BUTTON_Mol,
  BUTTON_Cavity,
  BUTTON_Accessible,
  PANEL_IsoInput,
  TEXT_Iso,
  TEXT_IsoCtrl,
  BUTTON_ResetCamera,
  LIST_Cavity
};

#endif
