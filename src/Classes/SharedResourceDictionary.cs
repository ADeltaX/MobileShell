using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;

namespace MobileShell.Classes
{
    public class SharedResourceDictionary : ResourceDictionary
    {
        private static bool isInDesignerMode;

        private Uri sourceUri;

        static SharedResourceDictionary() => isInDesignerMode = (bool)DesignerProperties.
                                            IsInDesignModeProperty.GetMetadata(typeof(DependencyObject)).DefaultValue;

        public static Dictionary<Uri, ResourceDictionary> SharedDictionaries { get; } = new Dictionary<Uri, ResourceDictionary>();

        public new Uri Source
        {
            get => sourceUri;

            set
            {
                sourceUri = value;

                if (!SharedDictionaries.ContainsKey(value) || isInDesignerMode)
                {
                    base.Source = value;

                    if (!isInDesignerMode)
                        SharedDictionaries.Add(value, this);
                }
                else
                {
                    this.MergedDictionaries.Add(SharedDictionaries[value]);
                }
            }
        }
    }
}