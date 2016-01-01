object fmPortSetup: TfmPortSetup
  Left = 789
  Top = 363
  Width = 280
  Height = 280
  Caption = 'Port setup'
  Color = clBtnFace
  Constraints.MaxHeight = 280
  Constraints.MaxWidth = 280
  Constraints.MinHeight = 280
  Constraints.MinWidth = 280
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object lbPortNameTag: TLabel
    Left = 32
    Top = 16
    Width = 25
    Height = 13
    Caption = 'Port: '
  end
  object lbBaudrate: TLabel
    Left = 32
    Top = 48
    Width = 43
    Height = 13
    Caption = 'Baudrate'
  end
  object lbDataBits: TLabel
    Left = 32
    Top = 80
    Width = 42
    Height = 13
    Caption = 'Data bits'
  end
  object lbStopBits: TLabel
    Left = 32
    Top = 112
    Width = 41
    Height = 13
    Caption = 'Stop bits'
  end
  object lbParity: TLabel
    Left = 32
    Top = 144
    Width = 26
    Height = 13
    Caption = 'Parity'
  end
  object lbFlowCtrl: TLabel
    Left = 32
    Top = 176
    Width = 58
    Height = 13
    Caption = 'Flow Contral'
  end
  object cbBaudrate: TComboBox
    Left = 96
    Top = 48
    Width = 81
    Height = 21
    ItemHeight = 13
    TabOrder = 0
    Text = '38400'
    OnChange = cbBaudrateChange
    Items.Strings = (
      'Custom'
      '110'
      '300'
      '600'
      '1200'
      '2400'
      '4800'
      '9600'
      '14400'
      '19200'
      '38400'
      '56000'
      '57600'
      '115200'
      '128000'
      '')
  end
  object cbDataBits: TComboBox
    Left = 96
    Top = 80
    Width = 49
    Height = 21
    Enabled = False
    ItemHeight = 13
    ItemIndex = 3
    TabOrder = 1
    Text = '8'
    Items.Strings = (
      '5'
      '6'
      '7'
      '8')
  end
  object cbStopBits: TComboBox
    Left = 96
    Top = 112
    Width = 49
    Height = 21
    Enabled = False
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 2
    Text = '1'
    Items.Strings = (
      '1'
      '1.5'
      '2')
  end
  object cbParity: TComboBox
    Left = 96
    Top = 144
    Width = 73
    Height = 21
    Enabled = False
    ItemHeight = 13
    TabOrder = 3
    Text = 'none'
    Items.Strings = (
      'None'
      'Odd'
      'Even'
      'Mark'
      'Space')
  end
  object btnOK: TButton
    Left = 104
    Top = 216
    Width = 57
    Height = 17
    Caption = 'OK'
    TabOrder = 4
    OnClick = btnOKClick
  end
  object cbPortList: TComboBox
    Left = 96
    Top = 16
    Width = 65
    Height = 21
    ItemHeight = 13
    TabOrder = 5
    OnChange = cbPortListChange
  end
  object cbFlowCtrl: TComboBox
    Left = 96
    Top = 176
    Width = 81
    Height = 21
    Enabled = False
    ItemHeight = 13
    ItemIndex = 2
    TabOrder = 6
    Text = 'None'
    Items.Strings = (
      'Hardware'
      'Software'
      'None'
      'Custom')
  end
  object btnPortScan: TButton
    Left = 184
    Top = 16
    Width = 57
    Height = 17
    Caption = 'ReScan'
    TabOrder = 7
    OnClick = btnPortScanClick
  end
end
