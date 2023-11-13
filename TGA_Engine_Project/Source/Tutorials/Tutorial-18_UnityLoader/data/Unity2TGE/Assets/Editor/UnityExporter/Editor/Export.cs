using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using Unity.VisualScripting;
using UnityEditor.ShaderGraph.Serialization;
using Newtonsoft.Json;
using System.Dynamic;
using static UnityEditor.PlayerSettings;
using static UnityEngine.Rendering.DebugUI.Table;
using UnityEngine.UIElements;

public class MenuItems
{
    [MenuItem("Tools/Export all #_x")]
    private static void NewMenuOption()
    {
        string path = EditorUtility.SaveFilePanel("Export scene...", "../", "exportedLevelASCII.txt", "txt");

        if (path.Length != 0)
        {
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(path))
            {
                GameObject[] allObjects = UnityEngine.Object.FindObjectsOfType<GameObject>();
                GameObject[] rootObjects = UnityEngine.SceneManagement.SceneManager.GetActiveScene().GetRootGameObjects();

                dynamic export = new ExpandoObject();
                export.project_root = Application.productName;
                export.scene_objects = new List<dynamic>(rootObjects.Length);

                foreach (GameObject go in rootObjects)
                {
                    dynamic obj = Traverse(go);
                    
                    export.scene_objects.Add(obj);
                }
                
                string json = JsonConvert.SerializeObject(export);
                file.WriteLine(json);
            }

        }
    }
    private static dynamic Traverse(GameObject go)
    {
        dynamic obj = new ExpandoObject();

        //Vector3 rot = go.transform.rotation.eulerAngles;
        Quaternion rot = go.transform.rotation;
        Vector3 pos = go.transform.position;
        Vector3 scale = go.transform.localScale;

        // we use anonymous object, they work similar to json in many ways, and is simple to serialize to json! just what we need ^^
        { // You can more or less put any data you want as long as it keeps serializable (i.e. can be converted to text, or json in our case)
            //obj.rotation = new { x = rot.x, y = rot.y, z = rot.z };
            obj.rotation = new { x=rot.x, y=rot.y, z=rot.z, w=rot.w };
            obj.position = new { x = pos.x, y = pos.y, z = pos.z };
            obj.scale = new { x = scale.x, y = scale.y, z = scale.z };
            obj.tag = go.tag;
            obj.name = go.name;
        }
        // Above is common for all things, they have name and transform

        // Some special cases, for example the Camera we want information about field of view and the near and far clipping plane
        { // since we are using a dynamic object we can simply add the information if we happen to find a camera component!
            var component = go.GetComponent<Camera>();
            if (component)
            {
                obj.type = "camera";    // object type is used in TGE to identify what data we can assume each object to have

                // Unity fov is always vertical, we need it horizontal!
                obj.fov = Camera.VerticalToHorizontalFieldOfView(component.fieldOfView, component.aspect);
                obj.far = component.farClipPlane;
                obj.near = component.nearClipPlane;
            }
        }
        { // For light sources you might want some information, [[-- this is left for your to figure out :) --]]
            var component = go.GetComponent<Light>();
            if (component)
            {
                obj.type = "light";
            }
        }
        { // Sprites are handled different from 3D-models
            var component = go.GetComponent<SpriteRenderer>();
            if (component)
            {
                obj.type = "sprite";

                obj.pivot = new
                {
                    x = component.sprite.pivot.x / component.sprite.texture.width,
                    y = component.sprite.pivot.y / component.sprite.texture.height
                };

                obj.path = Application.productName + "/" + AssetDatabase.GetAssetPath(component.sprite);
            }
        }
        { // Models has some special data
            var component = go.GetComponent<MeshFilter>();
            if (component)
            {
                obj.type = "model";
                obj.path = Application.productName + "/" + AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(component));
            }
        }
        { // And to find the correct assets for Animated models we need to do like this
            var component = go.GetComponent<Animator>();
            if (component)
            {
                obj.type = "animated_model";
                obj.path = Application.productName + "/" + AssetDatabase.GetAssetPath(PrefabUtility.GetCorrespondingObjectFromOriginalSource(component));
            }
        }
        
        if (go.transform.childCount > 0)
        {
            obj.children = new List<dynamic>(go.transform.childCount);
            foreach(Transform child in go.transform)
            {
                obj.children.Add(Traverse(child.gameObject));
            }
        }
        return obj;
    }
}
