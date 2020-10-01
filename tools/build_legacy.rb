require 'rubygems'
require 'json'
require 'FileUtils'

INPUT_PATH = "/Users/daigosato/Development/odencat/Splash/Sample/Animations"
OUTPUT_PATH = "/Users/daigosato/Development/odencat/splash-ebiten/cmd/splash-ebiten-player/assets/animations"

AnchorData = {
    "bottomLeft" => [-1, 1],
    "bottomCenter" => [0, 1],
    "bottomRight" => [1, 1],
    "centerLeft" => [-1, 0],
    "center" => [0, 0],
    "centerRight" => [1, 0],
    "topLeft" => [-1, -1],
    "topCenter" => [0, -1],
    "topRight" => [1, -1]
};


def to_pascal(str)
  if str
    str[0,1] = str[0,1].upcase
  end
  return str;
end

def build_animation_file(filename)
    data = ""
    File.open(filename, 'r') do |f|
       data = f.read
    end
    json = JSON.parse(data)


    typeString = ["normal", "background"]

    result = {}
    result["timelines"], result["dependencies"] = parse_timelines(json["keyframes"])
    result["events"] = json["events"]

    typeIndex = json["type"] ? json["type"] : 0
    result["type"] = typeString[typeIndex]
    return result
end

def parse_timelines(timelines)
    result = []
    dependencies = {"images"=>[], "animations"=>[]}
    timelines.each do |timelineIndex, timeline|
        result << parse_timeline(timeline, dependencies)
    end
    
    return result, dependencies
end

def parse_timeline(timeline, dependencies)
    result = {
        "alpha" => [],
        "position" => [],
        "rotation" => [],
        "scale" => [],
        "color" => [],
        "source" => []
    }

    latestSourceData = {"textureRect" => nil, "sourcePath" => nil, "maxFrame" => 1}
    timeline.each_with_index do |keyframe_set, i|
        last = timeline.length() -1 == i
        parse_keyframes(latestSourceData, keyframe_set, result, dependencies, last)
    end

    return result
end

def parse_keyframes(latestSourceData, keyframe_set, result, dependencies, last)
    frameNo = keyframe_set["frameNo"]

    if (keyframe_set["isEmpty"])
        result.each do |key, obj|
            if result[key].length > 0
                result[key] << {"frameNo" => frameNo, "wait" => true}
            end
        end
        latestSourceData["textureRect"] = nil
        latestSourceData["sourcePath"] = nil
    else
        # Add keyframe for each attribute in this keyframe set
        createAttributeKey(result, "alpha", keyframe_set, frameNo, 1.0)
        createAttributeKey(result, "position", keyframe_set, frameNo, [0, 0])
        createAttributeKey(result, "rotation", keyframe_set, frameNo, 0)
        createAttributeKey(result, "scale", keyframe_set, frameNo, [1, 1])
        createAttributeKey(result, "color", keyframe_set, frameNo, [0, 0, 0])
        createSourceKey(result, keyframe_set, frameNo, latestSourceData, dependencies, last);
    end
end

# Create keyframe for attributes(Scale, Alpha, Rotation ... )
def createAttributeKey(result, key, keyframe_set, frameNo, defaultValue)
    data = {}

    value = keyframe_set[key]
    tweenType = keyframe_set[key + "Tween"]

    if !tweenType.nil?
        data["tween"] = tweenType
    end

    if tweenType && tweenType != "none"
        data["frameNo"] = frameNo
        data["value"] = (value != nil) ? value : defaultValue
        
        # Position attribute has special options
        if key == "position"
            data["positionType"] = keyframe_set["positionType"] ? to_pascal(keyframe_set["positionType"]) : "None"

            data["positionAnchor"] = AnchorData[keyframe_set["positionTypeOption"]] ? AnchorData[keyframe_set["positionTypeOption"]] : AnchorData["center"]
        end
        if key == "rotation"
            data["facingOption"] = keyframe_set["facingOption"] ? to_pascal(keyframe_set["facingOption"]) : "None"
        end
        
    end
    
    unless data.empty?
        result[key] << data
    end
end

# Create keyframe for source key
def createSourceKey(result, keyframe_set, frameNo, latestSourceData, dependencies,last)
    # Source is somewhat special (Like, no lenear tween)
    rectDiff = latestSourceData["textureRect"] != keyframe_set["textureRect"]
    pathDiff = latestSourceData["sourcePath"] != keyframe_set["sourcePath"]
    priorityDiff = latestSourceData["priority"] != keyframe_set["priority"]
    blendTypeDiff = latestSourceData["blendType"] != keyframe_set["blendType"]
    
    latestSourceData["maxFrame"] = frameNo
    if rectDiff || pathDiff || priorityDiff || blendTypeDiff || last
        latestSourceData["textureRect"] = keyframe_set["textureRect"]
        latestSourceData["sourcePath"] = keyframe_set["sourcePath"]
        latestSourceData["priority"] = keyframe_set["priority"]
        latestSourceData["blendType"] = to_pascal(keyframe_set["blendType"])

        id = keyframe_set["sourcePath"].split(".")[0]
        ext = keyframe_set["sourcePath"].split(".")[1]
        sourceKeyFrame = {"frameNo" => frameNo, "id" => id}
        sourceKeyFrame["priority"] = keyframe_set["priority"] ? keyframe_set["priority"] : 0.5
        
        if keyframe_set["blendType"]
            sourceKeyFrame["blendType"] = to_pascal(keyframe_set["blendType"])
        end

        if ext == "png"
            #Images
            unless dependencies["images"].index(id)
                dependencies["images"] << id
            end
            sourceKeyFrame["type"] = "Image"
            sourceKeyFrame["rect"] = keyframe_set["textureRect"] ? keyframe_set["textureRect"] : [0,0,0,0]
            # Center
            center = [0, 0]
            if (keyframe_set["center"])
                center = keyframe_set["center"]
            end
            sourceKeyFrame["center"] = center
            
        else
            # Animations
            unless dependencies["animations"].index(id)
                dependencies["animations"] << id
            end
            sourceKeyFrame["type"] = "Animation"
            if keyframe_set["emitter"]
                sourceKeyFrame["emitter"] = true
                sourceKeyFrame["maxEmitAngle"] = keyframe_set["maxEmitAngle"]
                sourceKeyFrame["maxEmitSpeed"] = keyframe_set["maxEmitSpeed"]
                sourceKeyFrame["minEmitAngle"] = keyframe_set["minEmitAngle"]
                sourceKeyFrame["minEmitSpeed"] = keyframe_set["minEmitSpeed"]
            else
                sourceKeyFrame["emitter"] = false
            end
        end
        
        result["source"] << sourceKeyFrame
    end
end

# Recursively gather image dependency information
def retrieveAllDependencies(animation_data, root_id)
    root = animation_data[root_id]
    if root["processed"]
        return root["data"]["dependencies"]
    else
        dependencies = root["data"]["dependencies"]
        root["data"]["dependencies"]["animations"].each do |child_id|
            child_dependencies = retrieveAllDependencies(animation_data, child_id)
            dependencies["animations"] = dependencies["animations"] | child_dependencies["animations"]
            dependencies["images"] = dependencies["images"] | child_dependencies["images"]
        end

        root["data"]["dependencies"] = dependencies
        root["processed"] = true
        return dependencies
    end
end 

#----------------------------------------------------------------------------
# Build Process Start
#----------------------------------------------------------------------------
animation_data = {}
list = Dir["#{INPUT_PATH}/**/*.ani"]
list.each do |filename|
    # Convert Splash animation file into lightweight json object
    result = build_animation_file(filename)

    # Get ID from file path
    id = filename.gsub(%r{^#{INPUT_PATH}/}, "").split(".")[0];
    animation_data[id] = {"data" => result, "processed" => false}
end

animation_data.keys.each do |id|
    # recursively gather image dependency information and add it to the animation data itself
    retrieveAllDependencies(animation_data, id)
    
    # Get output filename
    outputFilename = OUTPUT_PATH + "/" + id + ".json"
    
    # Create directories
    FileUtils.mkpath File.dirname(outputFilename)
    
    # Save it as a file
    File.open(outputFilename, 'w') do |f|
        #f.write(animation_data[id]["data"].to_json)
        f.write(JSON.pretty_generate(animation_data[id]["data"]))
    end
end

# File.open(OUTPUT_PATH + "animations.json", 'w') do |f|
#    f.write(animation_data.keys.to_json)
# end
